// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyAbilitySystemBlueprintLibrary.generated.h"

struct FGameplayTag;
struct FOnAttributeChangeData;

class UAbilitySystemComponent;
class UGameplayEffect;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameplayAttributeValueChange, const FOnAttributeChangeData&);

UCLASS()
class MMO_API UMyAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static FActiveGameplayEffectHandle ApplyEffectToTarget(TSubclassOf<UGameplayEffect> Effect, AActor* Target, float Level);

	UFUNCTION(BlueprintCallable)
	static FActiveGameplayEffectHandle ApplyEffectToASC(TSubclassOf<UGameplayEffect> Effect, UAbilitySystemComponent* AbilitySystemComponent, float Level);

	UFUNCTION(BlueprintPure)
	static FGameplayAttribute GetAscAttributeByTag(const FGameplayTag AttributeTag, const UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintPure)
	static float GetAttributeValueByTag(FGameplayTag AttributeTag,  const UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintPure)
	static UAbilitySystemComponent* GetControlledAbilitySystemComponent(const UUserWidget* Widget);

	UFUNCTION(BlueprintCallable)
	static void GiveAbilityToActor(TSubclassOf<UGameplayAbility> Ability, AActor* Target);
	
	static void BindOnAttributeChange(FGameplayTag AttributeTag, UAbilitySystemComponent* ASC, const TFunction<void(const FOnAttributeChangeData& ChangedAttribute)> Functor);
	static void BindOnAttributeChange(FGameplayAttribute Attribute, UAbilitySystemComponent* ASC, TFunction<void(const FOnAttributeChangeData& ChangedAttribute)> Functor);
	
};
