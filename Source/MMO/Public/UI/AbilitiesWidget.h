#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitiesWidget.generated.h"

struct FGameplayTag;
class UAbilitiesPanelData;

UCLASS()
class MMO_API UAbilitiesWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddUpdatingTags(FGameplayTagContainer Tags);
	void OnTagUpdate(FGameplayTag Tag, int32);
};
