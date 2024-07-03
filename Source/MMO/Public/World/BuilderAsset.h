// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "World/CellBuildInfoAsset.h"
#include "BuilderAsset.generated.h"



UCLASS(BlueprintType)
class MMO_API UBuilderAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UCellBuildInfoAsset>> CellsBuildData;
};
