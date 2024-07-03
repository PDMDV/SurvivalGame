// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemContainer.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


class AItem;

USTRUCT()
struct FContainersTemplate
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ContainerTag;

	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> SlotsTags;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UPROPERTY(EditDefaultsOnly)
	TArray<FContainersTemplate> InventoryTemplate;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Inventory)
	TArray<AItemContainer*> Inventory;

	UPROPERTY(EditAnywhere)
	bool KeepCollisions = false;

	UFUNCTION(BlueprintCallable)
	bool AddItemToContainer(AItem* Item, FGameplayTag ContainerTag);

	UFUNCTION(BlueprintPure)
	AItemContainer* GetContainerByTag(FGameplayTag ContainerTag);

	UFUNCTION(BlueprintCallable)
	bool RemoveItem(AItem* Item);

	UFUNCTION(BlueprintPure)
	bool ContainsItemInContainer(AItem* Item, FGameplayTag ContainerTag);
	
	UFUNCTION(BlueprintPure)
	bool ContainsItem(const AItem* Item);

	UFUNCTION(BlueprintPure)
	bool HasContainer(FGameplayTag ContainerTag) const;
	
	UFUNCTION(BlueprintPure)
	AItem* GetFirstItemFromContainer(FGameplayTag ContainerTag);

	UFUNCTION(BlueprintPure)
	TArray<AItem*> GetItemsFromContainer(FGameplayTag ContainerTag);

	UFUNCTION(BlueprintPure)
	int32 GetContainerSize(FGameplayTag ContainerTag);
	
	UFUNCTION(BlueprintPure)
	bool IsContainerEmpty(FGameplayTag ContainerTag);

	UFUNCTION(BlueprintPure)
	static FName GetContainerCarryingSocketName(FGameplayTag ContainerTag);
	void Init();

	UFUNCTION(BlueprintPure)
	static FGameplayTagContainer GetContainersForItem(const AItem* Item);

	UFUNCTION(BlueprintPure)
	bool HasContainerFreeSpace(FGameplayTag ContainerTag);
	
	UFUNCTION(BlueprintPure)
	bool CanPickUpItem(const AItem* Item);

	UFUNCTION(BlueprintCallable)
	bool SwitchItemContainer(AItem* Item, FGameplayTag From, FGameplayTag To);

	UFUNCTION(BlueprintCallable)
	void DropItem(AItem* Item);

	UFUNCTION(BlueprintPure)
	static bool IsItemInInventory(AItem* Item);

	UFUNCTION(BlueprintPure)
	static UInventoryComponent* GetItemInventory(AItem* Item);

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Inventory(TArray<AItemContainer*> Previous);
};
