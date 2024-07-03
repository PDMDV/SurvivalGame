
#include "Game/MyGameMode.h"

#include "Character/MyPlayerController.h"
#include "Game/MyGameLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "World/WorldMap.h"

AMyGameMode::AMyGameMode()
{
}

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AMyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	RestartPlayer(NewPlayer);
	UMyGameLibrary::GetWorldMap(this)->AddRenderInvoker(NewPlayer->GetPawn());
}
