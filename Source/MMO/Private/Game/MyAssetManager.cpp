
#include "Game/MyAssetManager.h"
#include "AbilitySystemGlobals.h"

void UMyAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}