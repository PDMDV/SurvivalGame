
#include "Character/InventoryComponent.h"
#include "Game Objects/Item.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	Inventory.Empty();
}

FGameplayTagContainer UInventoryComponent::GetContainersForItem(const AItem* Item)
{
	FGameplayTagContainer ItemTagContainer;
	Item->GetOwnedGameplayTags(ItemTagContainer);

	return ItemTagContainer.Filter(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("CarryingSocket")));
}

bool UInventoryComponent::HasContainerFreeSpace(const FGameplayTag ContainerTag)
{
	if(const AItemContainer* Container = GetContainerByTag(ContainerTag))
	{
		return Container->HasFreeSpace();
	}
	return false;
}

bool UInventoryComponent::CanPickUpItem(const AItem* Item)
{
	FGameplayTagContainer ItemTagContainers = GetContainersForItem(Item);
	for(const FGameplayTag ItemTagContainer : ItemTagContainers )
	{
		if(!HasContainerFreeSpace(ItemTagContainer))
		{
			return false;
		}
	}
	return true;
}

bool UInventoryComponent::SwitchItemContainer(AItem* Item, FGameplayTag From, FGameplayTag To)
{
	if(!GetOwner()->HasAuthority()) return false;
	
	if(ContainsItemInContainer(Item, From) && HasContainerFreeSpace(To))
	{
		GetContainerByTag(From)->RemoveItem(Item);
		AddItemToContainer(Item, To);
		return true;
	}
	return false;
}

void UInventoryComponent::DropItem(AItem* Item)
{
	if(!GetOwner()->HasAuthority() || !IsValid(Item)) return;
	
	RemoveItem(Item);
	Item->OnStateChanged.Broadcast(Item, EItemStateChange::Droped);
}

bool UInventoryComponent::IsItemInInventory(AItem* Item)
{
	if(UInventoryComponent* InventoryComponent = Item->GetOwner()->GetComponentByClass<UInventoryComponent>())
	{
		return InventoryComponent->ContainsItem(Item);
	}
	return false;
}

UInventoryComponent* UInventoryComponent::GetItemInventory(AItem* Item)
{
	if(AActor* Owner = Item->GetOwner())
	{
		return Owner->GetComponentByClass<UInventoryComponent>();
	}
	return nullptr;
}

bool UInventoryComponent::AddItemToContainer(AItem* Item, FGameplayTag ContainerTag)
{
	if(!GetOwner()->HasAuthority() || !Item) return false;
	
	AItemContainer* ItemContainer = GetContainerByTag(ContainerTag);
	if(ItemContainer && ItemContainer->HasFreeSpace())
	{
		UInventoryComponent* PreviousInventory = GetItemInventory(Item);
		if(PreviousInventory != this)
		{
			if(PreviousInventory )
			{
				PreviousInventory->RemoveItem(Item);
				Item->OnStateChanged.Broadcast(Item, EItemStateChange::ChangedOwner);
			}
			else
			{
				Item->OnStateChanged.Broadcast(Item, EItemStateChange::PickedUp);
			}
		}
		const FGameplayTag Slot = ItemContainer->AddItem(Item);
		Item->AttachTo(GetOwner(), Slot, KeepCollisions);
		Item->SetOwner(GetOwner());
		return true;
	}
	return false;
}

AItemContainer* UInventoryComponent::GetContainerByTag(FGameplayTag ContainerTag)
{
	for(AItemContainer* Container : Inventory)
	{
		if(Container && Container->ContainerTag == ContainerTag)
		{
			return Container;
		}
	}
	return nullptr;
}

bool UInventoryComponent::RemoveItem(AItem* Item)
{
	if(!IsValid(Item) || !GetOwner()->HasAuthority()) return false;

	bool Success = false;
	for(AItemContainer* Container : Inventory)
	{
		if(Container->RemoveItem(Item))
		{
			Success = true;
		}
	}
	if(Success)
	{
		Item->SetOwner(nullptr);
		Item->Drop();
	}
	return Success;
}

bool UInventoryComponent::ContainsItemInContainer(AItem* Item, FGameplayTag ContainerTag)
{
	if(HasContainer(ContainerTag))
	{
		return GetContainerByTag(ContainerTag)->HasItem(Item);
	}
	return false;
}

bool UInventoryComponent::ContainsItem(const AItem* Item)
{
	for(const AItemContainer* ItemContainer : Inventory )
	{
		if(ItemContainer->HasItem(Item))
		{
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::HasContainer(const FGameplayTag ContainerTag) const
{
	for(const AItemContainer* Container : Inventory)
	{
		if(Container && Container->ContainerTag == ContainerTag)
		{
			return true;
		}
	}
	return false;
}

AItem* UInventoryComponent::GetFirstItemFromContainer(FGameplayTag ContainerTag)
{
	TArray<AItem*> ItemContainer = GetItemsFromContainer(ContainerTag);
	if(!ItemContainer.IsEmpty())
	{
		return ItemContainer[0];
	}
	return nullptr;
}

TArray<AItem*> UInventoryComponent::GetItemsFromContainer(FGameplayTag ContainerTag)
{
	TArray<AItem*> Result;
	if(AItemContainer* Container = GetContainerByTag(ContainerTag))
	{
		for(AItem* Item : Container->Items)
		{
			if(Item != nullptr)
			{
				Result.Add(Item);
			}
		}
	}
	return Result;
}

int32 UInventoryComponent::GetContainerSize(FGameplayTag ContainerTag)
{
	return GetItemsFromContainer(ContainerTag).Num();
}

bool UInventoryComponent::IsContainerEmpty(FGameplayTag ContainerTag)
{
	return GetContainerSize(ContainerTag) == 0;
}

FName UInventoryComponent::GetContainerCarryingSocketName(FGameplayTag ContainerTag)
{
	return ContainerTag.GetTagName();
}

void UInventoryComponent::Init()
{
	if(GetOwner()->HasAuthority())
	{
		Inventory.Empty();
		for(FContainersTemplate TemplateContainer : InventoryTemplate)
		{
			AItemContainer* ItemContainer = GetWorld()->SpawnActorDeferred<AItemContainer>(AItemContainer::StaticClass(), FTransform());
			ItemContainer->ContainerTag = TemplateContainer.ContainerTag;
			ItemContainer->SetOwner(GetOwner());
			for(FGameplayTag SlotTemplate : TemplateContainer.SlotsTags)
			{
				ItemContainer->Slots.Add(SlotTemplate);
				ItemContainer->Items.Add(nullptr);
			}
			ItemContainer->FinishSpawning(FTransform());
			Inventory.Add(ItemContainer);
		}
	}
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Inventory)
}

void UInventoryComponent::OnRep_Inventory(TArray<AItemContainer*> Previous)
{
}
