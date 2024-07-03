
#include "Ability System/MyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Ability System/CharacterAttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Character/BaseCharacter.h"

FActiveGameplayEffectHandle UMyAbilitySystemBlueprintLibrary::ApplyEffectToTarget(const TSubclassOf<UGameplayEffect> Effect, AActor* Target, const float Level)
{
	if (!Effect || !Target)
	{
		return FActiveGameplayEffectHandle(INDEX_NONE);
	}
	
	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
	{
		return ApplyEffectToASC(Effect, AbilitySystemComponent, Level);
	}
	return FActiveGameplayEffectHandle(INDEX_NONE);
}

FActiveGameplayEffectHandle UMyAbilitySystemBlueprintLibrary:: ApplyEffectToASC(const TSubclassOf<UGameplayEffect> Effect, UAbilitySystemComponent* AbilitySystemComponent, const float Level)
{
	if (!Effect || !AbilitySystemComponent)
		return FActiveGameplayEffectHandle(INDEX_NONE);

	const FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, Level, EffectContextHandle);
	return AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

FGameplayAttribute UMyAbilitySystemBlueprintLibrary::GetAscAttributeByTag(const FGameplayTag AttributeTag, const UAbilitySystemComponent* AbilitySystemComponent)
{
	const UCharacterAttributeSet* CharacterAttributeSet = Cast<UCharacterAttributeSet>(AbilitySystemComponent->GetAttributeSet(UCharacterAttributeSet::StaticClass()));
	return CharacterAttributeSet->GetAttributeByTag(AttributeTag);
}

float UMyAbilitySystemBlueprintLibrary::GetAttributeValueByTag(const FGameplayTag AttributeTag,  const UAbilitySystemComponent* AbilitySystemComponent)
{
	const UAttributeSet* AttributeSet = AbilitySystemComponent->GetAttributeSet(UAttributeSet::StaticClass());
	const FGameplayAttribute Attribute = GetAscAttributeByTag(AttributeTag, AbilitySystemComponent);
	return Attribute.GetNumericValue(AttributeSet);
}

UAbilitySystemComponent* UMyAbilitySystemBlueprintLibrary::GetControlledAbilitySystemComponent(const UUserWidget* Widget)
{
	if (const APlayerController* PlayerController = Widget->GetOwningPlayer())
	{
		if (const ABaseCharacter* ControlledCharacter = Cast<ABaseCharacter>(PlayerController->GetPawn()))
		{
			return ControlledCharacter->GetAbilitySystemComponent();
		}
	}
	return nullptr;
}

void UMyAbilitySystemBlueprintLibrary::GiveAbilityToActor(const TSubclassOf<UGameplayAbility> Ability, AActor* Target)
{
	if(!Target->HasAuthority()) return;

	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
	{
		const FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

void UMyAbilitySystemBlueprintLibrary::BindOnAttributeChange(const FGameplayTag AttributeTag, UAbilitySystemComponent* ASC, const TFunction<void(const FOnAttributeChangeData& ChangedAttribute)> Functor)
{
	const FGameplayAttribute Attribute = GetAscAttributeByTag(AttributeTag, ASC);
	BindOnAttributeChange(Attribute, ASC, Functor);
}

void UMyAbilitySystemBlueprintLibrary::BindOnAttributeChange(FGameplayAttribute Attribute, UAbilitySystemComponent* ASC, TFunction<void(const FOnAttributeChangeData& ChangedAttribute)> Functor)
{
	FOnGameplayAttributeValueChange& OnAttributeChangeDelegate = ASC->GetGameplayAttributeValueChangeDelegate(Attribute);
	OnAttributeChangeDelegate.AddLambda(Functor);
}