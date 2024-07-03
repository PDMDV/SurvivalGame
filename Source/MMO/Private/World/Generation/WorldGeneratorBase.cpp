
#include "World/Generation/WorldGeneratorBase.h"

#include "World/Generation/MapGeneratorModifier.h"
#include "World/WorldMap.h"

UWorldGeneratorBase::UWorldGeneratorBase()
{
}

void UWorldGeneratorBase::BuildMap(FMapData& Map, const FIntPoint& Position)
{
	if(!Map.HasChunk(Position))
	{
		Map.AddChunk(Position, FChunkData());
		GenerateChunk(Map.GetChunkData(Position), Position);
	}
}

void UWorldGeneratorBase::GenerateChunk(FChunkData* Chunk, const FIntPoint& Position)
{
	for (const auto& ModClass : Modifiers)
	{
		UMapGeneratorModifier* Modifier = NewObject<UMapGeneratorModifier>(this, ModClass);
		Modifier->Apply(*Chunk, Position);
	}
}

bool UWorldGeneratorBase::IsAnyObjectInRadius(const FVector& Center, const float Radius)
{
	for (auto ObjectPos : ObjectsPositions)
	{
		if (FVector::Distance(Center, ObjectPos) < Radius)
		{
			return true;
		}
	}
	return false;
}