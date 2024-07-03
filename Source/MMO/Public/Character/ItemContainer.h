#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Info.h"
#include "ItemContainer.generated.h"

class AItem;

UCLASS()
class MMO_API AItemContainer : public AInfo
{
	GENERATED_BODY()

	AItemContainer();

public:
	UPROPERTY(VisibleAnywhere, Replicated)
	FGameplayTag ContainerTag;

	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<FGameplayTag> Slots;

	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<AItem*> Items;
	
	UPROPERTY(EditAnywhere)
	bool InfiniteContainer = false;
	
	int32 GetUsedSlotNum() const;
	
	int32 GetSlotsNum() const;
	
	int32 GetFreeSlotsNum() const;

	bool HasFreeSpace() const;
	
	int32 GetFirstFreeSlotIndex();
	
	TArray<int32> GetSlotIndexWithItem(const AItem* Item);
	
	bool HasItem(const AItem* Item) const;

	FGameplayTag AddItem(AItem* Item);
	
	bool RemoveItem(AItem* Item);
	
protected:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
