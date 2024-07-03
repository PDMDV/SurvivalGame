
#include "Abilities/MyGameplayAbility.h"
#include "AbilitySystemComponent.h"

void UMyGameplayAbility::SendGameplayEventWithHitResult(const FGameplayTag EventTag, const FHitResult& HitResult)
{
	FGameplayEventData Payload;
	if(const IGameplayTagAssetInterface* Target = Cast<IGameplayTagAssetInterface>(HitResult.GetActor()))
	{
		Target->GetOwnedGameplayTags(Payload.TargetTags);
	}
	if(const IGameplayTagAssetInterface* Instigator = Cast<IGameplayTagAssetInterface>(GetOwningActorFromActorInfo()))
	{
		Instigator->GetOwnedGameplayTags(Payload.InstigatorTags);
	}
	
	Payload.EventTag = EventTag;
	Payload.Instigator = GetOwningActorFromActorInfo();
	Payload.Target = HitResult.GetActor();

	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData->HitResult = HitResult;
	Payload.TargetData.Add(TargetData);
	
	SendGameplayEvent(EventTag, Payload);
}

void UMyGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->CancelAbilities(&AtTheEndCancelAbilitiesWithTag);
}
