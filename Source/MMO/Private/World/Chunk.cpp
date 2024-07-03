
#include "World/Chunk.h"

#include "Algo/AllOf.h"
#include "World/WorldMap.h"
#include "World/Terrain.h"
#include "World/DataLayerHook.h"

#include "Net/UnrealNetwork.h"

AChunk::AChunk()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AChunk::BeginPlay()
{
	Super::BeginPlay();
}

void AChunk::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

 	DOREPLIFETIME(AChunk, Floors);
}

void AChunk::Destroyed()
{
	Super::Destroyed();

	for (ATerrain* Floor : Floors)
	{
		if( IsValid(Floor))
			Floor->Destroy();
	}
}

void AChunk::SetPosition(const FIntPoint& Position)
{
	GridPosition = Position;
	SetActorLocation(FVector(Position.X * AWorldMap::GetChunkWidth(), Position.Y * AWorldMap::GetChunkWidth(), 0.f));
}

bool AChunk::IsReady() const
{
	return Algo::AllOf(Floors, [](const ATerrain* Floor)
	{
		if(Floor)
		{
			return Floor->IsReady();
		}
		return false;
	});
}

AWorldMap* AChunk::GetWorldMap() const
{
	return WorldMap;
}

void AChunk::SetFloorsVisibility(int32 From, int32 To, const bool bVisibility)
{
	if (From > To)
	{
		std::swap(From, To);
	}
	for (int i = From; i <= To; i++)
	{
		if (Floors.IsValidIndex(i) && Floors[i])
			Floors[i]->SetVisibility(bVisibility);
	}
}

void AChunk::SetWorldMap(AWorldMap* Map)
{
	WorldMap = Map;
}

void AChunk::Init()
{
	if (!TerrainClass || !PCGVolumeClass) return;

	for (int32 FloorIndex = 0; FloorIndex < FChunkData::GetChunkHeight(); FloorIndex++)
	{
		ATerrain* Terrain = GetWorld()->SpawnActorDeferred<ATerrain>(TerrainClass, FTransform(), this);
		Terrain->SetPosition({GridPosition.X,GridPosition.Y,FloorIndex});
		Terrain->WorldMap = WorldMap;
		Terrain->FinishSpawning(FTransform());
		Floors.Push(Terrain);
		Terrain->AddActorLocalOffset(FVector(0, 0, 300.f * FloorIndex));
		Terrain->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

ADataLayerHook* AChunk::GetHook() const
{
	return Cast<ADataLayerHook>(GetOwner());
}

void AChunk::TryDestroy()
{
	Destroy();
}

void AChunk::KeepTryingDestroy(AActor* Context)
{
	Context->GetWorldTimerManager().SetTimer(DestroyingTimer, this, &ThisClass::TryDestroy, 0.1f, true);
}

