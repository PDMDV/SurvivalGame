
#include "Character/CharacterData.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/BaseCharacter.h"
#include "Ability System/MyAbilitySystemBlueprintLibrary.h"
#include "Game Objects/Item.h"

void UCharacterData::ApplyToCharacter(ABaseCharacter* Character)
{
	if(!Character->HasAuthority()) return;
	
	GiveDefaultEffects(Character);
	GiveDefaultAbilities(Character);
	GiveDefaultItems(Character);
}

void UCharacterData::GiveDefaultEffects(ABaseCharacter* Character)
{
	for (const TSubclassOf<UGameplayEffect> Effect : DefaultEffects)
	{
		UMyAbilitySystemBlueprintLibrary::ApplyEffectToASC(Effect, Character->GetAbilitySystemComponent(), 1.0f);
	}
}

void UCharacterData::GiveDefaultAbilities(ABaseCharacter* Character)
{
	for(const TSubclassOf<UGameplayAbility> Ability : DefaultAbilities)
	{
		UMyAbilitySystemBlueprintLibrary::GiveAbilityToActor(Ability, Character);
	}
}

void UCharacterData::GiveDefaultItems(ABaseCharacter* Character)
{
	for(FTaggedItem TaggedItem : DefaultItems)
	{
		if(AItem* Item = Character->GetWorld()->SpawnActor<AItem>(TaggedItem.Item))
		{
			FGameplayEventData EventData;
			EventData.EventTag = FGameplayTag::RequestGameplayTag("Ability.PickUp");
			EventData.Instigator = Character;
			EventData.Target = Item;
			Character->GetOwnedGameplayTags(EventData.InstigatorTags);
			Item->GetOwnedGameplayTags(EventData.TargetTags);
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, EventData.EventTag,EventData);
		}
	}
}

TArray<UAnimMontage*> UCharacterData::GiveMontagesWithTag(const FGameplayTag Tag)
{
	TArray<UAnimMontage*> Results;
	for(FTaggedMontage TaggedMontage : Montages)
	{
		if(TaggedMontage.Tag == Tag)
		{
			Results.Push(TaggedMontage.Montage);
		}
	}
	return Results;
}
