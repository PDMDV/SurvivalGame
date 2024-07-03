// Fill out your copyright notice in the Description page of Project Settings.

#include "World/Generation/TerrainCorrectModifier.h"

#include "World/WorldMap.h"
#include <array>

std::array<TerrainType, 4> UTerrainCorrectModifier::GetNeighbors(const int32 X, const int32 Y, const FChunkFloor& Floor)
{
	return {
		Floor.GetType(X + 0, Y + 1),
		Floor.GetType(X + 0, Y - 1),
		Floor.GetType(X + 1, Y + 0),
		Floor.GetType(X - 1, Y + 0),
	};
}

bool UTerrainCorrectModifier::IsTerrainAlone(const int32 X, const int32 Y, const FChunkFloor& Floor)
{
	for (const auto Type : GetNeighbors(X, Y, Floor))
	{
		if (Type == Floor.GetType(X, Y))
		{
			return false;
		}
	}
	return true;
}

void UTerrainCorrectModifier::Apply(FChunkData& Chunk, const FIntPoint& Position)
{
	const auto Size = FChunkData::GetChunkFloorSize();
	for (auto& Floor : Chunk.ChunkData)
	{
		for (int32 x = 0; x < Size.X; x++)
		{
			for (int32 y = 0; y < Size.Y; y++)
			{
				if (IsTerrainAlone(x, y, Floor))
				{
					Floor.SetType(x, y, Floor.GetType(x, y + 1));
				}
			}
		}
	}
}
