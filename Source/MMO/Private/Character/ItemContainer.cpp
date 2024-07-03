// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ItemContainer.h"
#include "Algo/Count.h"
#include "Character/InventoryComponent.h"
#include "Game Objects/Item.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

AItemContainer::AItemContainer()
{
	bReplicates = true;
}

int32 AItemContainer::GetUsedSlotNum() const
{
	return Algo::CountIf(Items, [](const AItem* Item)
	{
		return Item != nullptr;
	});
}

int32 AItemContainer::GetSlotsNum() const
{
	return Slots.Num();
}

int32 AItemContainer::GetFreeSlotsNum() const
{
	return GetSlotsNum() - GetUsedSlotNum();
}

bool AItemContainer::HasFreeSpace() const
{
	return GetFreeSlotsNum() > 0;
}

int32 AItemContainer::GetFirstFreeSlotIndex()
{
	return Items.Find(nullptr);
}

TArray<int32> AItemContainer::GetSlotIndexWithItem(const AItem* InItem)
{
	TArray<int32> Result;
	int32 Index = 0;
	for(const AItem* Item : Items)
	{
		if(Item == InItem)
		{
			Result.Add(Index);
		}
		Index++;
	}
	return Result;
}

bool AItemContainer::HasItem(const AItem* Item) const
{
	return Items.Contains(Item);
}

FGameplayTag AItemContainer::AddItem(AItem* Item)
{
	const int32 Index = GetFirstFreeSlotIndex();
	if(Index != INDEX_NONE)
	{
		Items[Index] = Item;
		return Slots[Index];
	}
	return FGameplayTag();
}

bool AItemContainer::RemoveItem(AItem* Item)
{
	TArray<int32> Indexes = GetSlotIndexWithItem(Item);
	bool Success = false;
	for(const int32 Index : Indexes)
	{
		Items[Index] = nullptr;
		Success = true;
	}
	return Success;
}

void AItemContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemContainer, ContainerTag)
	DOREPLIFETIME(AItemContainer, Slots)
	DOREPLIFETIME(AItemContainer, Items)
}
