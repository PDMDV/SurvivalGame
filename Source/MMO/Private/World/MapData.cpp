
#include "World/MapData.h"

#include "Game/MyGameLibrary.h"

FChunkFloor& FMapData::GetFloorData(FIntVector Position)
{
	return GetChunkData({Position.X, Position.Y})->ChunkData[Position.Z];
}

void FMapData::AdjustPosition(int32& LocalPosition, int32& ChunkPosition)
{
	if (LocalPosition < 0)
	{
		ChunkPosition += ((LocalPosition + 1) / GetChunkGridWidth()) - 1;
		LocalPosition = LocalPosition - (GetChunkGridWidth() * ChunkPosition);
	}
	else
	{
		ChunkPosition += LocalPosition / GetChunkGridWidth();
		LocalPosition = LocalPosition % GetChunkGridWidth();
	}
}

FIntVector FMapData::GetInChunkPosition(FIntVector Position)
{
	FIntPoint ChunkPosition = {0,0};
	AdjustPosition(Position.X, ChunkPosition.X);
	AdjustPosition(Position.Y, ChunkPosition.Y);
	return Position;
}

TArray<FColor> FChunkFloor::CreateColorArray() const
{
	TArray<FColor> ColorArray;
	ColorArray.SetNum(GetSize().X * GetSize().Y);

	for (int32 x = GetSize().X - 1; x >= 0; x--)
		for (int32 y = 0; y < GetSize().Y; y++)
		{
			const uint8 ArrayIndex = UMyGameLibrary::GetTerrainArrayIndex(GetType(x, y));
			ColorArray[x + y * GetSize().Y] = FColor(ArrayIndex, ArrayIndex, ArrayIndex);
		}
	return ColorArray;
}

TArray<TerrainType> FChunkFloor::CreateTerrainTypeArray() const
{
	TArray<TerrainType> TerrainArray;
	TerrainArray.SetNum(GetSize().X * GetSize().Y);

	for (int32 x = GetSize().X - 1; x >= 0; x--)
		for (int32 y = 0; y < GetSize().Y; y++)
		{
			TerrainArray[x + y * GetSize().Y] = GetType(x,y);
		}
	return TerrainArray;
}

TerrainType FMapData::GetType(FIntPoint ChunkPosition, int32 X, int32 Y, const int32 Z) const
{
	AdjustPosition(X, ChunkPosition.X);
	AdjustPosition(Y, ChunkPosition.Y);
	if (Chunks.Contains(ChunkPosition))
	{
		return Chunks.Find(ChunkPosition)->GetType(X, Y, Z);
	}
	return TerrainType::Invalid;
}

TerrainType FMapData::GetType(const FIntVector Position) const
{
	return GetType({0,0}, Position.X,Position.Y,Position.Z);
}
