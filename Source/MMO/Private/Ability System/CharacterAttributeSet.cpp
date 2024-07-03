
#include "Ability System/CharacterAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "GameplayTagsModule.h"

void UCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, const float InValue)
{
	if (Attribute == GetSpeedAttribute())
	{
		if (const ACharacter* Owner = Cast<ACharacter>(GetOwningActor()))
		{
			Owner->GetCharacterMovement()->MaxWalkSpeed = InValue;
		}
	}
	if (Attribute == GetHealthAttribute())
	{
		if(GetHealth() <= 0)
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), FGameplayTag::RequestGameplayTag("Ability.Death"), FGameplayEventData());
		}
	}
}

void UCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	if (Attribute == GetBalanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxBalance());
	}
}

void UCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, Health,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MaxHealth,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, Speed,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, Stamina,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MaxStamina,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, Balance,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MaxBalance,	COND_None, REPNOTIFY_Always);
}

void UCharacterAttributeSet::PostInitProperties()
{
	Super::PostInitProperties();

	const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	//TODO Try to create data asset with this data
	const FGameplayTag HealthTag = TagsManager.RequestGameplayTag(FName("Attribute.Health"));
	AttributesByTag.Add(HealthTag, GetHealthAttribute() );

	const FGameplayTag MaxHealthTag = TagsManager.RequestGameplayTag(FName("Attribute.MaxHealth"));
	AttributesByTag.Add(MaxHealthTag, GetMaxHealthAttribute() );

	const FGameplayTag StaminaTag = TagsManager.RequestGameplayTag(FName("Attribute.Stamina"));
	AttributesByTag.Add(StaminaTag, GetStaminaAttribute() );

	const FGameplayTag MaxStaminaTag = TagsManager.RequestGameplayTag(FName("Attribute.MaxStamina"));
	AttributesByTag.Add(MaxStaminaTag, GetMaxStaminaAttribute() );

	const FGameplayTag BalanceTag = TagsManager.RequestGameplayTag(FName("Attribute.Balance"));
	AttributesByTag.Add(BalanceTag, GetBalanceAttribute() );

	const FGameplayTag MaxBalanceTag = TagsManager.RequestGameplayTag(FName("Attribute.MaxBalance"));
	AttributesByTag.Add(MaxBalanceTag, GetMaxBalanceAttribute() );
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
	if (Data.EvaluatedData.Attribute == GetBalanceAttribute())
	{
		SetBalance(FMath::Clamp(GetBalance(), 0.f, GetMaxBalance()));
	}
}

FGameplayAttribute UCharacterAttributeSet::GetAttributeByTag(const FGameplayTag AttributeTag) const
{
	return AttributesByTag[AttributeTag];
}

void UCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, Health, OldHealth);
}

void UCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxHealth, OldMaxHealth);
}

void UCharacterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, Stamina, OldStamina);
}

void UCharacterAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxStamina, OldMaxStamina);
}

void UCharacterAttributeSet::OnRep_Balance(const FGameplayAttributeData& OldBalance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, Balance, OldBalance);
}

void UCharacterAttributeSet::OnRep_MaxBalance(const FGameplayAttributeData& OldMaxBalance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxBalance, OldMaxBalance);
}

void UCharacterAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, Speed, OldSpeed);
}
