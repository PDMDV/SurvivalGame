
#include "World/DataLayerHook.h"

ADataLayerHook::ADataLayerHook()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADataLayerHook::BeginPlay()
{
	Super::BeginPlay();
}

bool ADataLayerHook::IsFree() const
{
	return bIsFree;
}

void ADataLayerHook::SetIsFree(const bool bInIsFree)
{
	bIsFree = bInIsFree;
}
