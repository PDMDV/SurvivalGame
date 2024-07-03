#include "World/ChunkStreaming.h"

#include "World/DataLayerHook.h"

#include "Kismet/GameplayStatics.h"
#include "WorldPartition/DataLayer/WorldDataLayers.h"
#include "GameFramework/PlayerController.h"
#include <Math/MathFwd.h>

#include "Algo/RemoveIf.h"
#include "Net/UnrealNetwork.h"

UChunkStreaming::UChunkStreaming()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UChunkStreaming::BeginPlay()
{
	Super::BeginPlay();
}

void UChunkStreaming::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UChunkStreaming::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UChunkStreaming, ReplicatedChunks);
}

bool UChunkStreaming::TakeAllDataLayerHooks()	
{
	TArray<AActor*> Results;
	UGameplayStatics::GetAllActorsOfClass(GetOwner(), ADataLayerHook::StaticClass(), Results);
	if(Results.Num() > 0)
	{
		FreeDataLayerHooks.Reset();
		UsedDataLayerHooks.Reset();
		Chunks.Reset();
		for (auto Result : Results)
		{
			if (!FreeDataLayerHooks.Contains(Result))
			{
				FreeDataLayerHooks.Push(Cast<ADataLayerHook>(Result));
			}
		}
		return true;
	}
	return false;
}

void UChunkStreaming::RemoveHook(ADataLayerHook* Hook)
{
	FreeDataLayerHooks.Remove(Hook);
}

ADataLayerHook* UChunkStreaming::AddChunk(const FIntPoint& ChunkPosition)
{
	if (FreeDataLayerHooks.Num() > 0)
	{
		Chunks.Add(ChunkPosition, FreeDataLayerHooks.Last()->GetDataLayerInstances()[0]);
		ReplicatedChunks.Add({ChunkPosition, FreeDataLayerHooks.Last()->GetDataLayerInstances()[0]});
		UsedDataLayerHooks.Add(ChunkPosition, FreeDataLayerHooks.Last());
		return FreeDataLayerHooks.Pop();
	}
	return nullptr;
}

TArray<FIntPoint> UChunkStreaming::GetActiveChunks()
{
	return GetChunksWithState(EDataLayerRuntimeState::Activated);
}

TArray<FIntPoint> UChunkStreaming::GetLoadedChunks()
{
	return GetChunksWithState(EDataLayerRuntimeState::Loaded);
}

TArray<FIntPoint> UChunkStreaming::GetChunksWithState(const EDataLayerRuntimeState State)
{
	TArray<FIntPoint> ResultChunks;
	for (auto [ChunkPosition, DataLayer] : ReplicatedChunks)
	{
		if (GetDataLayerState(DataLayer) == State)
		{
			ResultChunks.Push(ChunkPosition);
		}
	}
	return ResultChunks;
}

EDataLayerRuntimeState UChunkStreaming::GetDataLayerState(const UDataLayerInstance* DataLayer) const
{
	return WorldDataLayers->GetDataLayerRuntimeStateByName(DataLayer->GetDataLayerFName());
}

void UChunkStreaming::UnloadChunk(const FIntPoint& ChunkPosition)
{
	ChangeChunkState(ChunkPosition, EDataLayerRuntimeState::Unloaded);
}

void UChunkStreaming::ActiveChunk(const FIntPoint& ChunkPosition)
{
	ChangeChunkState(ChunkPosition, EDataLayerRuntimeState::Activated);
}

void UChunkStreaming::LoadChunk(const FIntPoint& ChunkPosition)
{
	ChangeChunkState(ChunkPosition, EDataLayerRuntimeState::Loaded);
}

void UChunkStreaming::ReleaseChunk(const FIntPoint& ChunkPosition)
{
	if(!UsedDataLayerHooks.Contains(ChunkPosition)) return;

	ActiveChunk(ChunkPosition);
	ADataLayerHook* Hook = UsedDataLayerHooks[ChunkPosition];
	FreeDataLayerHooks.Push(Hook);
	UsedDataLayerHooks.Remove(ChunkPosition);
	Chunks.Remove(ChunkPosition);
	Algo::RemoveIf(ReplicatedChunks, [ChunkPosition](const FChunkDataLayer& Data) {return Data.Chunk == ChunkPosition;});
	
}

void UChunkStreaming::ChangeChunkState(const FIntPoint& ChunkPosition, const EDataLayerRuntimeState State)
{
	if (Chunks.Contains(ChunkPosition))
	{
		const UDataLayerInstance* DataLayerInstance = Chunks[ChunkPosition];
		WorldDataLayers->SetDataLayerRuntimeState(DataLayerInstance, State);
	}
}

bool UChunkStreaming::HasChunk(const FIntPoint& ChunkPosition)
{
	for (auto[ Position, Layer] : Chunks)
	{
		if (Position == ChunkPosition)
			return true;
	}
	return false;
}

int32 UChunkStreaming::GetFreeHooksNum() const
{
	return FreeDataLayerHooks.Num();
}

