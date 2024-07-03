// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterAttributeBars.h"

#include "AbilitySystemComponent.h"
#include "Ability System/MyAbilitySystemBlueprintLibrary.h"
#include "UI/AttributeBarsData.h"

void UCharacterAttributeBars::BindOnAttributeChange(const FGameplayTag AttributeTag)
{
	UAbilitySystemComponent* AbilitySystemComponent = UMyAbilitySystemBlueprintLibrary::GetControlledAbilitySystemComponent(this);
	UMyAbilitySystemBlueprintLibrary::BindOnAttributeChange(AttributeTag, AbilitySystemComponent,
		[=](const FOnAttributeChangeData& ChangedAttribute)
		{
			AttributeChange(AttributeTag, ChangedAttribute.NewValue, ChangedAttribute.OldValue);
		});
}

void UCharacterAttributeBars::InitializeAttribute(const FGameplayTag AttributeTag)
{
	const UAbilitySystemComponent* AbilitySystemComponent = UMyAbilitySystemBlueprintLibrary::GetControlledAbilitySystemComponent(this);
	const float AttributeValue = UMyAbilitySystemBlueprintLibrary::GetAttributeValueByTag(AttributeTag, AbilitySystemComponent);
	AttributeChange(AttributeTag, AttributeValue,AttributeValue);
}

void UCharacterAttributeBars::BindWidgetEventsToOnAttributeChangeDelegate()
{
	for(const UBarAsset* AttributeBarDataAsset : AttributeBarsDataAsset->BarsAssets)
	{
		BindOnAttributeChange(AttributeBarDataAsset->MaxAttributeTag);
		InitializeAttribute(AttributeBarDataAsset->MaxAttributeTag);
		
		BindOnAttributeChange(AttributeBarDataAsset->AttributeTag);
		InitializeAttribute(AttributeBarDataAsset->AttributeTag);
	}
}

void UCharacterAttributeBars::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	BindWidgetEventsToOnAttributeChangeDelegate();
}
