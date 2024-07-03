
#include "Character/BaseCharacter.h"

#include "Ability System/CharacterAttributeSet.h"
#include "Ability System/MyAbilitySystemComponent.h"
#include "Character/CharacterData.h"
#include "Character/InventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Game Objects/Item.h"
#include "Game/MyGameLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "World/WorldMap.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	ACharacter::SetReplicateMovement(true);

	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	InventoryComponent =  CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	InventoryComponent->SetNetAddressable();
	InventoryComponent->SetIsReplicated(true);
	
	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("Attribute Set"));
	
	GetCharacterMovement()->SetIsReplicated(true);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bReplicates = true;
}

void ABaseCharacter::WaitForSpawnReady()
{
	SetMovementActive(false);
	
	if(TimerHandle.IsValid() || !IsLocallyControlled()) return;
	
	AWorldMap* WorldMap = UMyGameLibrary::GetWorldMap(this);
	
	if(!WorldMap->IsChunkInPositionReady(GetActorLocation()))
	{
		FTimerDelegate Delegate;
		Delegate.BindLambda([WorldMap, this]()
		{
			if(WorldMap->IsChunkInPositionReady(GetActorLocation()))
			{
				SetMovementActive(true);
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			}
		});
		GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 0.5f, true );
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(bShouldWaitUntilSpawnIsReady)
	{
		WaitForSpawnReady();
	}
	if(CharacterData)
	{
		CharacterData->ApplyToCharacter(this);
	}
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

void ABaseCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCharacterAttributeSet* ABaseCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

FRotator ABaseCharacter::GetDesireDeltaRotation()
{
	return UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), GetControlRotation());
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABaseCharacter::GetOwnedGameplayTags(FGameplayTagContainer& InTagContainer) const
{
	AbilitySystemComponent->GetOwnedGameplayTags(InTagContainer);
}

void ABaseCharacter::AttachItemToSocket(AItem* Item, FName SocketName)
{
	Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
}

void ABaseCharacter::DetachItem(AItem* Item)
{
	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ABaseCharacter::SetMovementActive_Implementation(bool Active)
{
	UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	CharacterMovementComponent->DefaultLandMovementMode = (Active) ? MOVE_Walking : MOVE_None;
	CharacterMovementComponent->SetMovementMode((Active) ? MOVE_Walking : MOVE_None);
}

FRotator ABaseCharacter::GetVelocityRotator() const
{
	return UKismetMathLibrary::MakeRotFromX(GetVelocity());
}

float ABaseCharacter::GetRelativeToMovementYawRotation() const
{
	if(GetVelocity().Length() > 0.0f)
	{
		return UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), GetVelocityRotator()).Yaw;
	}
	return 0.0f;
}

FVector ABaseCharacter::GetSocketPosition(const FName SocketName) const
{
	return GetMesh()->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_Component).GetLocation();
}

