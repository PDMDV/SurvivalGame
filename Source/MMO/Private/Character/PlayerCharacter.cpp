
#include "Character/PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "World/WorldMap.h"
#include "Ability System/MyAbilitySystemComponent.h"
#include "Game/MyGameLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	GroundPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Ground Position"));
	GroundPosition->SetupAttachment(RootComponent);
	
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	bUseControllerRotationYaw = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	WorldMap = UMyGameLibrary::GetWorldMap(this);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsLocallyControlled() || HasAuthority())
	{
		const FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), DesireRotation, DeltaTime, 5.0);
		SetActorRotation(NewRotation);
	}
	
	if(IsLocallyControlled())
	{
		const FIntVector GridPosition = WorldMap->GetGridPosition(GetActorLocation());
		if (WorldMap->GetType(GridPosition) == TerrainType::Cave)
		{
			if (!bIsUnderground)
			{
				WorldMap->SetAllLevelsVisibility(GridPosition.Z + 1, GChunkZSize, false);
				bIsUnderground = true;
			}
		}
		else if (bIsUnderground)
		{
			WorldMap->SetAllLevelsVisibility(GridPosition.Z + 1, GChunkZSize, true);
			bIsUnderground = false;
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerCharacter::MultiplyCameraDistance(const float Distance)
{
	SpringArm->TargetArmLength *= Distance;
}

void APlayerCharacter::SetDesireRotation(FRotator InDesireRotation)
{
	DesireRotation = InDesireRotation;
}

