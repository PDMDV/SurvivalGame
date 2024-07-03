
#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "Abilities/MyGameplayAbility.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UInventoryComponent;
class UMyAbilitySystemComponent;
class UCharacterData;
class AItem;
class UGameplayEffect;
class UCharacterAttributeSet;

UCLASS()
class MMO_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
	FTimerHandle TimerHandle;

public:
	ABaseCharacter();
	
	void WaitForSpawnReady();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMyAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly)
	UCharacterData* CharacterData;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldWaitUntilSpawnIsReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& InTagContainer) const override;

	UFUNCTION(BlueprintPure)
	FRotator GetDesireDeltaRotation(); // Doesnt work on simulated proxy because Controller Rotation is not replicating

	UFUNCTION(BlueprintPure)
	float GetRelativeToMovementYawRotation() const;

	UFUNCTION(BlueprintPure)
	FVector GetSocketPosition(FName SocketName) const;

	UFUNCTION(BlueprintPure)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	void AttachItemToSocket(AItem* Item, FName SocketName);

	UFUNCTION(BlueprintCallable)
	void DetachItem(AItem* Item);

	UFUNCTION(Server, Reliable)
	void SetMovementActive(bool Active);

	UCharacterAttributeSet* GetAttributeSet() const;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	FRotator GetVelocityRotator() const;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
};
