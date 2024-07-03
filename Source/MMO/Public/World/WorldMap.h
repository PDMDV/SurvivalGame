#pragma once

#include "World/Chunk.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Windows/WindowsCriticalSection.h"
#include "World/MapData.h"
#include "World/Generation/WorldGeneratorBase.h"
#include "WorldMap.generated.h"

class UWorldGeneratorBase;
class UObjectsDeployment;
class UChunkStreaming;

USTRUCT()
struct FChunkMap
{
	GENERATED_BODY()

	FChunkMap() :Position(), Chunk(nullptr) {};
	FChunkMap(FIntPoint InPosition) : Position(InPosition), Chunk(nullptr) {};
	FChunkMap(FIntPoint InPosition, AChunk* InChunk) : Position(InPosition), Chunk(InChunk) {};

	UPROPERTY()
	FIntPoint Position;

	UPROPERTY()
	AChunk* Chunk;

	friend bool operator==(const FChunkMap& Lhs, const FChunkMap& RHS)
	{
		return Lhs.Position == RHS.Position;
	}

	friend bool operator!=(const FChunkMap& Lhs, const FChunkMap& RHS)
	{
		return !(Lhs == RHS);
	}
};

UCLASS()
class MMO_API AWorldMap : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldMap();

	const float RenderDistance = 7000.f;
	const float LoadDistance = 8000.f;
	const float GenerateDistance = 10000.f;
	const float DeactivateDistance = 12000.f;
	const float RemoveDistance = 16000.f;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UChunkStreaming> StreamingComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChunk> ChunkClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWorldGeneratorBase> GeneratorClass;

	UPROPERTY(VisibleAnywhere)
	TSet<const AActor*> RenderInvokers;

	UPROPERTY(VisibleAnywhere)
	FMapData WorldMap;
	
	UPROPERTY(VisibleAnywhere)
	TMap<FIntPoint, AChunk* > Chunks;

	UPROPERTY(Replicated)
	TArray<FChunkMap> ReplicatedChunks;

	UPROPERTY(VisibleAnywhere)
	bool bHookInitialized = false;

	FTimerHandle TimerHandle;
	FTimerHandle ManagerTimerHandle;
	FRWLock WorldLock;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UFUNCTION(CallInEditor)
	void CreateChunkInEditor();

	UFUNCTION(CallInEditor)
	void ResetChunksInEditor();

	UFUNCTION(CallInEditor)
	bool InitHooks();
	
	static FIntVector GetChunkSize() { return { GChunkSize, GChunkSize, GChunkZSize }; };
	static int32 GetChunkWidth() { return FMapData::GetChunkWidth(); };
	static int32 GetChunkGridWidth() { return FMapData::GetChunkGridWidth(); };
	static int32 GetGridCellLength() { return FMapData::GetGridCellLength(); };
	static FVector GetRealPosition(FIntVector Position);
	static FVector GetChunkPosition(const FIntPoint& ChunkPosition);
	static FVector GetChunkCenter(const FIntPoint& ChunkPosition);
	static void AdjustPosition(int32& LocalPosition, int32& ChunkPosition);
	static FIntPoint GetChunkInPosition(FIntVector Position);
	
	void SetLevelsVisibility(const FIntPoint& ChunkPosition, const int32 From, const int32 To, bool bVisibility);
	void SetAllLevelsVisibility(int32 From, int32 To, bool bVisibility);
	void RenderChunksInLocation(const FVector& Location, int32 MaxCreatedChunks = INT32_MAX);
	void GenerateNeighbourChunks(const FIntPoint& ChunkPosition);
	void RegisterChunk(FIntPoint ChunkPosition, AChunk* Chunk);
	void CreateChunk(const FIntPoint& ChunkPosition);
	void GenerateChunk(const FIntPoint& ChunkPosition);
	void UnloadChunk(const FIntPoint& Position) const;
	void LoadChunk(const FIntPoint& Position) const;
	void ActiveChunk(const FIntPoint& Position) const;
	void DestroyChunk(const FIntPoint& ChunkPosition);
	void ManageChunks();
	void AddRenderInvoker(const AActor* Invoker);
	bool RemoveRenderInvoker(const AActor* Invoker);
	bool IsChunkInPositionReady(FVector Position);

	FIntVector ConvertPositionToGridPosition(FVector Position);
	TerrainType GetType(const FIntPoint& ChunkPosition, int32 X, int32 y, int32 z) const;
	TerrainType GetType(int32 X, int32 Y, int32 Z) const;
	TerrainType GetType(FIntVector Position) const;
	TArray<FIntPoint> GetActiveChunks() const;
	TArray<FIntPoint> GetChunksInRadius(const FVector& Center, float Radius) const;
	FIntVector GetGridPosition(const FVector& Position) const;
	AChunk* GetChunk(const FIntPoint& Position);
	FVector GetPlayerPosition() const;
};
