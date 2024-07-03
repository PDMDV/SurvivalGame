
#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

class UCapsuleComponent;

UCLASS()
class MMO_API AWeapon : public AItem
{
	GENERATED_BODY()
public:

	AWeapon();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> BlockEffect;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCapsuleComponent> DamageArea;

	
	UFUNCTION(BlueprintCallable)
	void DrawDebugDamageArea() const;

	UFUNCTION(BlueprintCallable)
	void GetObjectsInDamageArea(TSubclassOf<AActor> ClassFilter, TArray<AActor*>& Results) const;

	UFUNCTION(BlueprintPure)
	TSubclassOf<UGameplayEffect> GetDamageEffect() const { return DamageEffect; };

	UFUNCTION(BlueprintPure)
	TSubclassOf<UGameplayEffect> GetBlockEffect() const { return BlockEffect; };
};
