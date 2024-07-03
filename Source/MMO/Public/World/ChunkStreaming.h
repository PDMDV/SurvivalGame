
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Map.h"
#include "ChunkStreaming.generated.h"

class ADataLayerHook;
enum class EDataLayerRuntimeState : uint8;

USTRUCT()
struct FChunkDataLayer
{
	GENERATED_BODY()
	
	UPROPERTY()
	FIntPoint Chunk;

	UPROPERTY()
	const UDataLayerInstance* DataLayer;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMO_API UChunkStreaming : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<ADataLayerHook*> FreeDataLayerHooks;

	UPROPERTY(VisibleAnywhere)
	TMap<FIntPoint, ADataLayerHook*> UsedDataLayerHooks;

	UPROPERTY(VisibleAnywhere)
	TMap<FIntPoint, const UDataLayerInstance*> Chunks;

	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<FChunkDataLayer> ReplicatedChunks;

	UPROPERTY(EditAnywhere)
	AWorldDataLayers* WorldDataLayers;

public:	
	UChunkStreaming();

protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void virtual GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	void UnloadChunk(const FIntPoint& ChunkPosition);
	void ActiveChunk(const FIntPoint& ChunkPosition);
	void LoadChunk(const FIntPoint& ChunkPosition);
	void ReleaseChunk(const FIntPoint& ChunkPosition);
	void ChangeChunkState(const FIntPoint& ChunkPosition, EDataLayerRuntimeState State);
	void RemoveHook(ADataLayerHook* Hook);

	TArray<FIntPoint> GetActiveChunks();
	TArray<FIntPoint> GetLoadedChunks();
	TArray<FIntPoint> GetChunksWithState(EDataLayerRuntimeState State);
	ADataLayerHook* AddChunk(const FIntPoint& ChunkPosition);
	EDataLayerRuntimeState GetDataLayerState(const UDataLayerInstance* DataLayer) const;
	int32 GetFreeHooksNum() const;
	bool TakeAllDataLayerHooks();
	bool HasChunk(const FIntPoint& ChunkPosition);
};
