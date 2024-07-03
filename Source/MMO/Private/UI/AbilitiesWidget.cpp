
#include "UI/AbilitiesWidget.h"

#include "AbilitySystemComponent.h"
#include "Character/PlayerCharacter.h"

void UAbilitiesWidget::AddUpdatingTags(FGameplayTagContainer Tags)
{
	if(const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		for(const FGameplayTag Tag : Tags)
		{
			PlayerCharacter->GetAbilitySystemComponent()->RegisterGameplayTagEvent(Tag).AddUObject(this, &ThisClass::OnTagUpdate);
		}
	}
}

void UAbilitiesWidget::OnTagUpdate(const FGameplayTag Tag, int32)
{
	PreConstruct(false);
}
