
#include "Character/PlayerCharacterAnimInstance.h"

#include "Character/InventoryComponent.h"
#include "Character/MyPlayerController.h"
#include "Character/PlayerCharacter.h"
#include "Game Objects/Item.h"
#include "Game/MyGameLibrary.h"
#include "GameFramework/PawnMovementComponent.h"

void UPlayerCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ABaseCharacter>(TryGetPawnOwner());
}

void UPlayerCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!Character) return;
	
	Speed = Character->GetMovementComponent()->Velocity.Length();
	YawRotation = Character->GetRelativeToMovementYawRotation();
	DesireYawRotationDelta = Character->GetDesireDeltaRotation().Yaw;
	VelocityRotator = Character->GetVelocityRotator();

	const FGameplayTag LeftHand = FGameplayTag::RequestGameplayTag("CarryingSocket.WeaponLeft");
	const FGameplayTag RightHand = FGameplayTag::RequestGameplayTag("CarryingSocket.WeaponRight");
	bIsCarrying = !Character->InventoryComponent->IsContainerEmpty(RightHand) || !Character->InventoryComponent->IsContainerEmpty(LeftHand);
	bCarryingTwoHanded = Character->InventoryComponent->GetFirstItemFromContainer(LeftHand) != nullptr && (Character->InventoryComponent->GetFirstItemFromContainer(LeftHand) == Character->InventoryComponent->GetFirstItemFromContainer(RightHand));

	if (bIsCarrying && bCarryingTwoHanded)
	{
		AItem* TwoHandedItem = Character->InventoryComponent->GetFirstItemFromContainer(LeftHand);
		FVector RightHandLocation =TwoHandedItem->GetSocketPosition("RightHand");
		FVector LeftHandLocation = TwoHandedItem->GetSocketPosition("LeftHand");

		FName SocketName = UMyGameLibrary::GetRandomOwningChildTag(TwoHandedItem, FGameplayTag::RequestGameplayTag("TwoHandedSocket")).GetTagName();
		const FVector ItemLocation = Character->GetSocketPosition(SocketName);
		RightHandSocket = ItemLocation + RightHandLocation;
		LeftHandSocket = ItemLocation + LeftHandLocation;
	}
}

void UPlayerCharacterAnimInstance::SetIsDeath(bool bInIsDeath)
{
	bIsDeath = bInIsDeath;
}
