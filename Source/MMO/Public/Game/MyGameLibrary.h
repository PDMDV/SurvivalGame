// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyGameLibrary.generated.h"

class AGameObject;
enum class TerrainType : uint8;

UCLASS()
class MMO_API UMyGameLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static bool IsInRadius(const AActor* First, const AActor* Second, float Distance);

	UFUNCTION(BlueprintPure)
	static AWorldMap* GetWorldMap(const UObject* Context);

	UFUNCTION(BlueprintPure)
	static TArray<UAnimMontage*> GetMontagesWithTag(ABaseCharacter* Character, const FGameplayTag Tag);

	UFUNCTION(BlueprintPure)
	static UAnimMontage* GetRandomMontageWithTag(ABaseCharacter* Character, const FGameplayTag Tag);

	
	static FGameplayTagContainer GetOwningChildTags(IGameplayTagAssetInterface* Owner, FGameplayTag Tag);
	static FGameplayTag GetRandomOwningChildTag(IGameplayTagAssetInterface* Owner, FGameplayTag Tag);
	static UTexture2D*  CreateTextureFromColorArray(TArray<FColor> Data, const FIntPoint& Size);
	static uint8 GetTerrainArrayIndex(TerrainType Type);
};
