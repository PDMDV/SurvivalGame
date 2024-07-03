
#include "Game Objects/Item.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Ability System/MyAbilitySystemBlueprintLibrary.h"
#include "Game/MyGameLibrary.h"
#include "Net/UnrealNetwork.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
	SetMobility(EComponentMobility::Movable);
	SetPhysicsAndCollisions(true, true);
	
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetStaticMeshComponent()->SetIsReplicated(true);
	AActor::SetReplicateMovement(true);
	bStaticMeshReplicateMovement = true;
	bReplicates = true;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItem, Attachment );
}

void AItem::OnRep_Attachment(FAttachment Previous)
{
	if(Attachment.Actor && Attachment.Socket.IsValid())
	{
		AttachTo(Attachment.Actor,Attachment.Socket, Attachment.bKeepCollisions );
	}
	if(!Attachment.Actor && !Attachment.Socket.IsValid())
	{
		Detach();
	}
}

void AItem::SetPhysicsAndCollisions(bool bActivePhysics, bool bActiveCollision)
{
	GetStaticMeshComponent()->SetSimulatePhysics(bActivePhysics);
	GetStaticMeshComponent()->SetEnableGravity(bActivePhysics);
	
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, (bActiveCollision)? ECR_Block : ECR_Ignore);
    GetStaticMeshComponent()->SetCollisionEnabled((bActiveCollision)? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::QueryOnly);
}

bool AItem::ApplyPickUpEffect(AActor* Actor)
{
	if(PickUpEffect && HasAuthority())
	{
		PickUpEffectHandle = UMyAbilitySystemBlueprintLibrary::ApplyEffectToTarget(PickUpEffect, Actor, 1.0f);
	}
	return PickUpEffectHandle.IsValid();
}

void AItem::RemovePickUpEffect(AActor* Actor)
{
	if(const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor))
	{
		UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
		if (HasAuthority() && AbilitySystemComponent)
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(PickUpEffectHandle);
		}
	}
}

bool AItem::ApplyCarryingEffect(AActor* Actor)
{
	if(CarryingEffect && HasAuthority())
	{
		CarryingEffectHandle = UMyAbilitySystemBlueprintLibrary::ApplyEffectToTarget(CarryingEffect, Actor, 1.0f);
	}
	return CarryingEffectHandle.IsValid();
}

void AItem::RemoveCarryingEffect(AActor* Actor)
{
	if(const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor))
	{
		UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
		if (HasAuthority() && AbilitySystemComponent)
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(CarryingEffectHandle);
		}
	}
}

bool AItem::IsAttached() const
{
	return IsValid(Attachment.Actor);
}

void AItem::AttachTo(AActor* Actor, FGameplayTag SocketTag, bool KeepCollisions)
{
	if(HasAuthority())
	{
		SetAttachment(Actor, SocketTag, KeepCollisions);
	}
	SetPhysicsAndCollisions(false, KeepCollisions);
	if(ACharacter* Character = Cast<ACharacter>(Actor))
	{
		if(HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Item.TwoHanded")))
		{
			FName SocketName = UMyGameLibrary::GetRandomOwningChildTag(this, FGameplayTag::RequestGameplayTag("TwoHandedSocket")).GetTagName();
			AttachToComponent(
				Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		}
		else
		{
			AttachToComponent(
			Character->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
			SocketTag.GetTagName());
		}
	}
	else
	{
		AttachToComponent(
			Actor->GetRootComponent(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
			SocketTag.GetTagName());
	}
}

void AItem::Detach()
{
	if(HasAuthority())
	{
		SetAttachment(nullptr, FGameplayTag(), false);
	}
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetPhysicsAndCollisions(true, true);
}

void AItem::SetAttachment(AActor* Actor, FGameplayTag Socket, bool KeepCollisions)
{
	Attachment.Actor = Actor;
	Attachment.Socket = Socket;
	Attachment.bKeepCollisions = KeepCollisions;
}

void AItem::Drop()
{
	RemoveCarryingEffect(Attachment.Actor);
	RemovePickUpEffect(Attachment.Actor);
	Detach();
}

void AItem::GetOwnedGameplayTags(FGameplayTagContainer& InTagContainer) const
{
	InTagContainer = TagContainer;
}

FVector AItem::GetSocketPosition(FName Name) const
{
	return GetStaticMeshComponent()->GetSocketTransform(Name, RTS_Actor).GetLocation() * GetStaticMeshComponent()->GetRelativeScale3D();
}
