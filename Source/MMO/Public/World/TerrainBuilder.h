#pragma once

#include "CoreMinimal.h"
#include "DynamicMeshActor.h"
#include "World/GridBuilder.h"
#include "TerrainBuilder.generated.h"

struct FMapData;
struct FGridBuilder;
struct FOrRequirements;
struct FRequirements;

class AWorldMap;
class UBuilderAsset;
class UDynamicMesh;
class UDynamicMeshComponent;
class ATerrain;
class UWorldGeneratorBase;

enum class TerrainType : uint8;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class MMO_API UTerrainBuilder : public UObject
{
	GENERATED_BODY()

public:
	FMapData* MapDataRef;

	UPROPERTY()
	FIntVector Position;

	UPROPERTY()
	float GridCellLength = 100.f;

	UPROPERTY()
	TArray<FGridBuilder> Builders;

	UPROPERTY()
	TArray<FGridBuilder> MeshBuffers;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UBuilderAsset>> CellsBuildDataAssets;
	
	UPROPERTY()
	TObjectPtr<UDynamicMesh> ProcessedMesh;
	
	UPROPERTY()
    TObjectPtr<UDynamicMesh> TemporaryHelpMesh;

	UPROPERTY()
	UDynamicMeshPool* MeshPool;
	
	void ApplyNoiseToGeometry(FIntVector InPosition);
	void Init();
	void BuildTerrainCell(const FIntVector InPosition, UBuilderAsset* BuildAsset, int Index);
	void Finish(UDynamicMeshComponent* MeshComponent);
	void Build(AWorldMap* InWorldMap, const FIntVector InPosition);
	bool HasRequirements(const FIntVector InPosition, const FRequirements& AndRequirements) const;
	bool HasOrRequirements(const FIntVector InPosition, const FOrRequirements& OrRequirements) const;
	bool HasAndRequirements(FIntVector InPosition, const FRequirements& Requirements) const;
	FVector GetRealPosition(const FIntPoint& GridPosition) const;
	TerrainType GetType(FIntVector InPosition) const;
};
