
#include "World/Generation/MapGeneratorModifier.h"
#include <Kismet/KismetRenderingLibrary.h>
#include <Materials/MaterialInterface.h>
#include <Engine/TextureRenderTarget2D.h>
#include "World/WorldMap.h"

void UMapGeneratorModifier::Apply(FChunkData& Chunk, const FIntPoint& Position)
{
	 
}

TArray<FColor> UMapGeneratorModifier::GetColorMapFromMaterial(UMaterialInterface* Material, UTextureRenderTarget2D* RenderTarget, const FIntPoint& Size, const float Height, const FIntPoint& Offset /*= {0,0}*/)
{
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	DynamicMaterial->SetScalarParameterValue("OffsetX", Offset.X);
	DynamicMaterial->SetScalarParameterValue("OffsetY", Offset.Y);
	DynamicMaterial->SetScalarParameterValue("height", Height);

	RenderTarget->ResizeTarget(Size.X, Size.Y);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, DynamicMaterial);
	FRenderTarget* RT = RenderTarget->GameThread_GetRenderTargetResource();
	TArray<FColor> ColorMap;
	RT->ReadPixels(ColorMap);

	return ColorMap;
}

void UMapGeneratorModifier::ModifyTerrainTypeByColor(TerrainType Type, TArray<TerrainType> OverwrittenTypes, FColor Color, FChunkFloor& ChunkFloor, const TArray<FColor>& Map)
{
	const FIntPoint MapSize = FChunkData::GetChunkFloorSize();

	for (int32 y = 0; y < MapSize.Y; y++)
		for (int32 x = 0; x < MapSize.X; x++)
		{
			const auto CurrentType = ChunkFloor.GetType(x,y);
			for (const auto OverwrittenType : OverwrittenTypes)
			{
				if (OverwrittenType == CurrentType && Map[x + y * MapSize.X] == Color)
				{
					ChunkFloor.SetType(x, y, Type);
				}
			}
		}
}

void UMapGeneratorModifier::ModifyTerrainTypeByNeighbours(const TerrainType InType, TArray<TerrainType> OverwrittenTypes, TArray<FNeighbourRequired> RequiredNeighbours, FChunkData& Chunk)
{
	for (int32 z = 0; z < Chunk.GetChunkHeight(); z++)
		for (int32 y = 0; y < Chunk.GetChunkFloorSize().Y; y++)
			for (int32 x = 0; x < Chunk.GetChunkFloorSize().X; x++)
			{
				const TerrainType CurrentType = Chunk.GetType(x, y, z);
				for (const auto OverwrittenType : OverwrittenTypes)
				{
					if (OverwrittenType == CurrentType)
					{
						for (const auto [type, position] : RequiredNeighbours)
						{
							if (Chunk.GetType(x + position.X, y + position.Y, z + position.Z) == type)
							{
								Chunk.SetType(x, y, z, InType);
							}
						}	
					}
				}
			}
}

