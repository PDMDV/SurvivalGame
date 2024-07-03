// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/DataAsset.h"
#include "AbilitiesPanelData.generated.h"

USTRUCT(BlueprintType)
struct FAbilityIconData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush Brush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayEventData EventData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText OnImageText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TooltipText;
};

UCLASS(BlueprintType)
class MMO_API UAbilitiesPanelData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAbilityIconData> Abilities;
};
