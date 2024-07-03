
#pragma once

#include "CoreMinimal.h"
#include "MapGeneratorModifier.generated.h"

struct FChunkData;
class UWorldGeneratorBase;
enum class TerrainType : uint8;
struct FChunkFloor;

USTRUCT(BlueprintType)
struct FNeighbourRequired
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TerrainType Type;

	UPROPERTY(EditAnywhere)
	FIntVector Position;
};

UCLASS(Blueprintable)
class MMO_API UMapGeneratorModifier : public UObject
{
	GENERATED_BODY()

public:
	virtual void Apply(FChunkData& Chunk, const FIntPoint& Position);
	TArray<FColor> GetColorMapFromMaterial(UMaterialInterface* Material, UTextureRenderTarget2D* RenderTarget, const FIntPoint& Size, float Height, const FIntPoint& Offset = {0,0});
	static void ModifyTerrainTypeByColor(TerrainType NewType, TArray<TerrainType> OverwrittenTypes, FColor Color, FChunkFloor& Chunk, const TArray<FColor>& Map);
	static void ModifyTerrainTypeByNeighbours(TerrainType InType, TArray<TerrainType> OverwrittenTypes, TArray<FNeighbourRequired> RequiredNeighbours, FChunkData& Chunk);
};
