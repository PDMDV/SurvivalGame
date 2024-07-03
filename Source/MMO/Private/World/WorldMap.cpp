
#include "World/WorldMap.h"

#include "World/Generation/WorldGeneratorBase.h"
#include "World/MapData.h"
#include "World/Chunk.h"
#include "World/ChunkStreaming.h"
#include "World/DataLayerHook.h"
#include "Character/PlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

AWorldMap::AWorldMap()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StreamingComponent = CreateDefaultSubobject<UChunkStreaming>(TEXT("Streaming Component"));\
}

void AWorldMap::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority)
	{
		ManageChunks();
		GetWorldTimerManager().SetTimer(ManagerTimerHandle, this, &ThisClass::ManageChunks, 0.5f, true);
	}
}

void AWorldMap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWorldMap, ReplicatedChunks);
}

void AWorldMap::CreateChunkInEditor()
{
	CreateChunk({0,0});
}

void AWorldMap::ResetChunksInEditor()
{
	for(auto MappedChunk : Chunks)
	{
		AChunk* Chunk = MappedChunk.Value;
		if(IsValid(Chunk))
		{
			Chunk->Destroy();
		}

	}
	Chunks.Reset();
	ReplicatedChunks.Reset();
	WorldMap.Chunks.Reset();
}

bool AWorldMap::InitHooks()
{
	return StreamingComponent->TakeAllDataLayerHooks();
}

void AWorldMap::UnloadChunk(const FIntPoint& Position) const
{
	StreamingComponent->UnloadChunk(Position);
}

void AWorldMap::LoadChunk(const FIntPoint& Position) const
{
	StreamingComponent->LoadChunk(Position);
}

void AWorldMap::ActiveChunk(const FIntPoint& Position) const
{
	StreamingComponent->ActiveChunk(Position);
}

void AWorldMap::DestroyChunk(const FIntPoint& ChunkPosition)
{
	AChunk* Chunk = Chunks[ChunkPosition];
	Chunk->Destroy();
}

FIntVector AWorldMap::GetGridPosition(const FVector& Position) const
{
	return FIntVector(FMath::FloorToInt(Position.X / GGridCellLength), FMath::FloorToInt(Position.Y / GGridCellLength), FMath::FloorToInt(Position.Z / 300.f));
}

FVector AWorldMap::GetRealPosition(const FIntVector Position)
{
	return FVector{ static_cast<float>(Position.X) * GGridCellLength + GGridCellLength / 2.f,
					static_cast<float>(Position.Y) * GGridCellLength + GGridCellLength / 2.f,
					static_cast<float>(Position.Z) * 300.f };
}

TerrainType AWorldMap::GetType(const FIntPoint& ChunkPosition, const int32 X, const int32 Y, const int32 Z) const
{
	return WorldMap.GetType(ChunkPosition, X, Y, Z);
}

TerrainType AWorldMap::GetType(const int32 X, const int32 Y, const int32 Z) const
{
	return WorldMap.GetType({X,Y,Z});
}

TerrainType AWorldMap::GetType(const FIntVector Position) const
{
	return WorldMap.GetType(Position);
}

void AWorldMap::SetLevelsVisibility(const FIntPoint& ChunkPosition, const int32 From, const int32 To, bool bVisibility)
{
	for(FChunkMap Value : ReplicatedChunks)
	{
		if(Value.Position == ChunkPosition && Value.Chunk)
		{
			Value.Chunk->SetFloorsVisibility(From, To, bVisibility);
		}
	}
}

void AWorldMap::SetAllLevelsVisibility(int32 From, int32 To, bool bVisibility)
{
	for (auto Chunk : GetActiveChunks())
	{
		SetLevelsVisibility(Chunk, From, To, bVisibility);
	}
}

TArray<FIntPoint> AWorldMap::GetActiveChunks() const
{
	return StreamingComponent->GetActiveChunks();
}

void AWorldMap::RenderChunksInLocation(const FVector& Location, const int32 MaxCreatedChunks)
{
	TArray<FIntPoint> ActiveChunks;
	const TArray<FIntPoint> GenerateChunks = GetChunksInRadius(Location, GenerateDistance);

	int32 CreatedChunks = 0;
	for (const FIntPoint Chunk : GenerateChunks)
	{
		if(!WorldMap.HasChunk(Chunk))
		{
			GenerateChunk(Chunk);
		}

		const AChunk* CurrentChunk = GetChunk(Chunk);
		if( FVector::Distance(GetChunkCenter(Chunk), Location) < LoadDistance)
		{
			if(!CurrentChunk && CreatedChunks < MaxCreatedChunks)
			{
				CreateChunk(Chunk);
				LoadChunk(Chunk);
				CreatedChunks++;
			}
			if(StreamingComponent->HasChunk(Chunk) && CurrentChunk)
			{
				if(FVector::Distance(GetChunkCenter(Chunk), Location) < RenderDistance)
				{
					ActiveChunk(Chunk);
					ActiveChunks.Push(Chunk);
				}
			}
		}
	}
}

void AWorldMap::ManageChunks()
{
	check(HasAuthority())

	if(!bHookInitialized)
	{
		bHookInitialized = InitHooks();
	}
	
	for(const AActor* Invoker : RenderInvokers)
	{
		RenderChunksInLocation(Invoker->GetActorLocation(), 1);
	}

	for(FIntPoint ActiveChunk : StreamingComponent->GetActiveChunks())
	{
		bool bDeactivate = true;
		for(const AActor* Invoker : RenderInvokers)
		{
			if(FVector::Distance(GetChunkCenter(ActiveChunk), Invoker->GetActorLocation()) < DeactivateDistance)
			{
				bDeactivate = false;
			}
		}
		if(bDeactivate)
		{
			LoadChunk(ActiveChunk);
		}
	}

	for(FIntPoint LoadedChunk : StreamingComponent->GetLoadedChunks())
	{
		bool bRemove = true;
		for(const AActor* Invoker : RenderInvokers)
		{
			if(FVector::Distance(GetChunkCenter(LoadedChunk), Invoker->GetActorLocation()) < RemoveDistance)
			{
				bRemove = false;
			}
		}
		if(bRemove)
		{
			StreamingComponent->ActiveChunk(LoadedChunk);
			StreamingComponent->ReleaseChunk(LoadedChunk);
			AChunk* CurrentChunk = GetChunk(LoadedChunk);
			if(CurrentChunk && !CurrentChunk->Destroy())
			{
				// It is impossible to destroy a chunk immediately after activation 
				CurrentChunk->KeepTryingDestroy(this);
			}
		}
	}
}

AChunk* AWorldMap::GetChunk(const FIntPoint& Position)
{
	if (Chunks.Contains(Position))
	{
		return Chunks[Position];
	}
	return nullptr;
}

FVector AWorldMap::GetChunkPosition(const FIntPoint& ChunkPosition)
{
	return { static_cast<float>(ChunkPosition.X * GetChunkWidth()), static_cast<float>(ChunkPosition.Y * GetChunkWidth()), 0.0f };
}

FVector AWorldMap::GetChunkCenter(const FIntPoint& ChunkPosition)
{
	return GetChunkPosition(ChunkPosition) + FVector{ GetChunkWidth() * 0.5f, GetChunkWidth() * 0.5f, 0.f };
}

void AWorldMap::AdjustPosition(int32& LocalPosition, int32& ChunkPosition)
{
	return FMapData::AdjustPosition(LocalPosition, ChunkPosition);
}

FIntPoint AWorldMap::GetChunkInPosition(FIntVector Position)
{
	FIntPoint ChunkPosition = { 0,0 };
	AdjustPosition(Position.X, ChunkPosition.X);
	AdjustPosition(Position.Y, ChunkPosition.Y);
	return ChunkPosition;
}

TArray<FIntPoint> AWorldMap::GetChunksInRadius(const FVector& Center, float Radius) const
{
	const int32 GridWidth = FMath::CeilToInt(Radius / (GGridCellLength * GChunkSize));
	const FIntPoint CentralChunk = GetChunkInPosition(GetGridPosition(Center));

	TArray<FIntPoint> Results;
	for (int y = CentralChunk.Y - GridWidth; y < CentralChunk.Y + GridWidth; y++)
		for (int x = CentralChunk.X - GridWidth; x < CentralChunk.X + GridWidth; x++)
		{
			if (FMath::Abs((GetChunkCenter({ x,y }) - Center).Length()) < Radius)
			{
				Results.Push({ x,y });
			}
		}
		return Results;
}

FVector AWorldMap::GetPlayerPosition() const
{
	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	const ACharacter* PlayerCharacter = Cast<ACharacter>(PlayerController->GetPawn());
	return PlayerCharacter ? PlayerCharacter->GetActorLocation() : FVector();
}

void AWorldMap::AddRenderInvoker(const AActor* Invoker)
{
	RenderInvokers.Add(Invoker);
}

bool AWorldMap::RemoveRenderInvoker(const AActor* Invoker)
{
	return static_cast<bool>(RenderInvokers.Remove(Invoker));
}

FIntVector AWorldMap::ConvertPositionToGridPosition(FVector Position)
{
	return FIntVector(
		Position.X / FMapData::GetGridCellLength(),
		Position.Y / FMapData::GetGridCellLength(),
		Position.Z / FMapData::GetGridCellLength());
}

bool AWorldMap::IsChunkInPositionReady(FVector Position)
{
	const FIntVector GridPosition = ConvertPositionToGridPosition(Position);
	const FIntPoint ChunkPosition = GetChunkInPosition(GridPosition);
	if(ReplicatedChunks.Contains(ChunkPosition))
	{
		AChunk* Chunk = ReplicatedChunks.FindByKey(ChunkPosition)->Chunk;
		if(Chunk)
		{
			return ReplicatedChunks.FindByKey(ChunkPosition)->Chunk->IsReady();
		}
	}
	return false;
}

void AWorldMap:: GenerateNeighbourChunks(const FIntPoint& ChunkPosition)
{
	for(int32 x = -1 ; x <= 1 ; x++)
		for (int32 y = -1; y <= 1; y++)
		{
			const FIntPoint CurrentChunk(FIntPoint{ x,y } + ChunkPosition);
			if (!WorldMap.HasChunk(CurrentChunk))
			{
				GenerateChunk(CurrentChunk);
			}
		}
}

void AWorldMap::RegisterChunk(FIntPoint ChunkPosition, AChunk* Chunk)
{
	if (Chunks.Contains(ChunkPosition))
	{
		Chunks[ChunkPosition] = Chunk;
	}
	else
	{
		Chunks.Add({ ChunkPosition }, Chunk);
		ReplicatedChunks.Add({ChunkPosition, Chunk});
	}
}

void AWorldMap::CreateChunk(const FIntPoint& ChunkPosition)
{
	check(HasAuthority());
	
	if (Chunks.Contains(ChunkPosition) && Chunks[ChunkPosition]) return;

	if(!WorldMap.HasChunk(ChunkPosition))
	{
		GenerateChunk(ChunkPosition);
	}
	
	GenerateNeighbourChunks(ChunkPosition);

	if (ADataLayerHook* Hook = StreamingComponent->AddChunk(ChunkPosition))
	{
		FActorSpawnParameters Parameters;
		AChunk* Chunk = GetWorld()->SpawnActorDeferred<AChunk>(ChunkClass, GetActorTransform(), Hook);
#if WITH_EDITOR
		Chunk->SetFolderPath("/Chunks");
#endif
		Chunk->SetWorldMap(this);
		Chunk->FinishSpawning(GetActorTransform());
		Chunk->SetPosition(ChunkPosition);
		Chunk->Init();
		RegisterChunk(ChunkPosition, Chunk);
	}
}

void AWorldMap::GenerateChunk(const FIntPoint& ChunkPosition)
{
	FRWScopeLock WriteLock(WorldLock, SLT_Write);
	if(!WorldMap.HasChunk(ChunkPosition))
	{
		UWorldGeneratorBase* Generator = NewObject<UWorldGeneratorBase>(this, GeneratorClass, TEXT("Generator"));
		Generator->BuildMap(WorldMap, ChunkPosition);
	}
}
