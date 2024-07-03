
#pragma once

#include "CoreMinimal.h"
#include "MapGeneratorModifier.h"
#include "TerrainCorrectModifier.generated.h"

struct FChunk;

UCLASS()
class MMO_API UTerrainCorrectModifier : public UMapGeneratorModifier
{
	GENERATED_BODY()

public:
	virtual void Apply(FChunkData& Chunk, const FIntPoint& Position) override;
	static std::array<TerrainType, 4> GetNeighbors(int32 X, int32 Y, const FChunkFloor& Floor);
	static bool IsTerrainAlone(int32 X, int32 Y, const FChunkFloor& Floor);
};
