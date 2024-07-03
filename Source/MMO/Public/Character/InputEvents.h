
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InputEvents.generated.h"

class UInputAction;
enum class ETriggerEvent : uint8;

USTRUCT(BlueprintType)
struct FInputTags
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Input;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Event")
	TMap<ETriggerEvent, FGameplayTag> Events;
};

UCLASS(BlueprintType)
class MMO_API UInputEvents : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<FInputTags> InputTags;
};
