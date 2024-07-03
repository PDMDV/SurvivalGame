
#pragma once

#include "CoreMinimal.h"
#include "MapGeneratorModifier.h"
#include "Containers/StaticArray.h"
#include "Containers/Map.h"
#include "TerrainGenerator.generated.h"

class UTextureRenderTarget2D;

USTRUCT(BlueprintType)
struct FTerrainModifyByColor
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TerrainType NewTerrain;

	UPROPERTY(EditAnywhere)
	TArray<TerrainType> OverwrittenTypes;

	UPROPERTY(EditAnywhere)
	FColor Color;
};

USTRUCT(BlueprintType)
struct FTerrainModifyByNeighbour
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TerrainType NewTerrain;

	UPROPERTY(EditAnywhere)
	TArray<TerrainType> OverwrittenTypes;

	UPROPERTY(EditAnywhere)
	TArray<FNeighbourRequired> RequiredNeighbours;
};

UCLASS()
class MMO_API UTerrainGenerator : public UMapGeneratorModifier
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditAnywhere, EditFixedSize) // Size = 8
	TArray<float> Heights = TArray({0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f });

	UPROPERTY(EditAnywhere)
	TArray<FTerrainModifyByColor> TerrainModifiersByColor;

	UPROPERTY(EditAnywhere)
	TArray<FTerrainModifyByNeighbour> TerrainModifiersByNeighbour;

	virtual void Apply(FChunkData& Chunk, const FIntPoint& Position) override;
};
