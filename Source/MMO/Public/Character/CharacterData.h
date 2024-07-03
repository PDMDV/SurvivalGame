
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CharacterData.generated.h"

class ABaseCharacter;
class AItem;
class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FTaggedItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag Tag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AItem> Item;
};

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag Tag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> Montage;
};

UCLASS()
class MMO_API UCharacterData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
    TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	TArray<FTaggedItem> DefaultItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimMontages")
	TArray<FTaggedMontage> Montages;

	UFUNCTION(BlueprintCallable)
	void ApplyToCharacter(ABaseCharacter* Character);

	UFUNCTION(BlueprintPure)
	TArray<UAnimMontage*> GiveMontagesWithTag(FGameplayTag Tag);
	
	void GiveDefaultEffects(ABaseCharacter* Character);
	void GiveDefaultAbilities(ABaseCharacter* Character);
	void GiveDefaultItems(ABaseCharacter* Character);
};
