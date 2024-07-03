
#include "Game Objects/EffectArea.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Ability System/MyAbilitySystemBlueprintLibrary.h"

UEffectArea::UEffectArea()
{
	UPrimitiveComponent::SetCollisionResponseToAllChannels(ECR_Ignore);
	UPrimitiveComponent::SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UEffectArea::BeginPlay()
{
	Super::BeginPlay();

	const bool HasAuthority = GetOwner()->HasAuthority();
	if (HasAuthority)
	{
		if(!OnComponentBeginOverlap.IsBound())
		{
			OnComponentBeginOverlap.AddDynamic(this, &UEffectArea::StartOverlapping);
		}
		if(!OnComponentEndOverlap.IsBound())
		{
			OnComponentEndOverlap.AddDynamic(this, &UEffectArea::StopOverlapping);
		}
	}
	SetGenerateOverlapEvents(HasAuthority);
}

void UEffectArea::BeginDestroy()
{
	RemoveAllEffects();
	Super::BeginDestroy();
}

void UEffectArea::RemoveAllEffects()
{
	for(TTuple<UAbilitySystemComponent*, FActiveGameplayEffectHandle> ActiveHandle : ActiveEffectHandles)
	{
		UAbilitySystemComponent* AbilitySystemComponent = ActiveHandle.Key;
		const FActiveGameplayEffectHandle EffectHandle = ActiveHandle.Value;
		if(IsValid(AbilitySystemComponent) && EffectHandle.IsValid())
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle, 1);
		}
	}
}

void UEffectArea::StartOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (!IsValid(OtherActor)) return;

	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		const FActiveGameplayEffectHandle ActiveEffectHandle = UMyAbilitySystemBlueprintLibrary::ApplyEffectToASC(Effect, AbilitySystemComponent, 1.0f);
		if(ActiveEffectHandle.IsValid())
		{
			ActiveEffectHandles.Add(AbilitySystemComponent, ActiveEffectHandle);
		}
	}
}

void UEffectArea::StopOverlapping( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor)) return;
	
	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		if(ActiveEffectHandles.Contains(AbilitySystemComponent))
		{
			const FActiveGameplayEffectHandle EffectHandle = ActiveEffectHandles[AbilitySystemComponent];
			AbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle, 1);
			ActiveEffectHandles.Remove(AbilitySystemComponent);
		}
	}
}
