
#include "Game/MyGameState.h"

#include "Kismet/GameplayStatics.h"
#include "World/WorldMap.h"

void AMyGameState::FindWorldMap()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, WorldMapClass, Actors);
	check(Actors.Num() == 1);
	WorldMap = Cast<AWorldMap>(Actors[0]);
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

	FindWorldMap();
}
