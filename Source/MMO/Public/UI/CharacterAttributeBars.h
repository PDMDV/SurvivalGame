
#pragma once

#include "CoreMinimal.h"
#include "AttributeBarsData.h"
#include "Blueprint/UserWidget.h"
#include "CharacterAttributeBars.generated.h"

struct FGameplayTag;
class UAttributeBarsData;
class UAbilitySystemComponent;

UCLASS()
class MMO_API UCharacterAttributeBars : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAttributeBarsData> AttributeBarsDataAsset;

	UFUNCTION(BlueprintCallable)
	void BindWidgetEventsToOnAttributeChangeDelegate();

	UFUNCTION(BlueprintImplementableEvent)
	void AttributeChange(FGameplayTag AttributeTag, float NewValue, float OldValue);

	void BindOnAttributeChange(const FGameplayTag AttributeTag);
	void InitializeAttribute(const FGameplayTag AttributeTag);

protected:
	virtual void NativeOnInitialized() override;
};

