// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "TagMap.generated.h"


UCLASS(BlueprintType)
class MMO_API UTagMap : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag to tag")
	TMap<FGameplayTag, FGameplayTag> BindingMap;
};
