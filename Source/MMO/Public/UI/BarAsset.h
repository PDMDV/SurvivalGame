// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Styling/SlateTypes.h"
#include "BarAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class MMO_API UBarAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTag MaxAttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bar")
	FProgressBarStyle Style;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bar")
	FVector2D Size = {100.f, 800.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bar")
	float LeftSpacerLength = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bar")
	float RightSpacerLength = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bar")
	float DownSpacerHeight = 20.f;
};
