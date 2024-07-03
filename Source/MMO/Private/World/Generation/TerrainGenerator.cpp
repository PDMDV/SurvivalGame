
#include "World/Generation/TerrainGenerator.h"
#include "World/WorldMap.h"

void UTerrainGenerator::Apply(FChunkData& Chunk, const FIntPoint& Position)
{
	const FIntPoint Offset = Position * Chunk.GetChunkFloorSize();
	int32 Index = 0;
	for (auto& ChunkFloor : Chunk.ChunkData)
	{
		if (TerrainModifiersByColor.Num() <= 0) break;

		TArray<FColor> ColorMap = GetColorMapFromMaterial(Material, RenderTarget, Chunk.GetChunkFloorSize(), Heights[Index++], Offset);
		for (const auto& [newTerrain, overwrittenTypes, color] : TerrainModifiersByColor)
		{
			ModifyTerrainTypeByColor(newTerrain, overwrittenTypes, color, ChunkFloor, ColorMap);
		}
	}

	for (const auto& [newTerrain, overwrittenTypes, requiredNeighbours] : TerrainModifiersByNeighbour)
	{
		ModifyTerrainTypeByNeighbours(newTerrain, overwrittenTypes, requiredNeighbours, Chunk);
	}
}
