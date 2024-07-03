
#include "World/TerrainBuilder.h"

#include "GeometryScript/GeometryScriptTypes.h"
#include "GeometryScript/MeshBasicEditFunctions.h"
#include "GeometryScript/MeshDecompositionFunctions.h"
#include "GeometryScript/MeshDeformFunctions.h"
#include "GeometryScript/SceneUtilityFunctions.h"
#include "World/WorldMap.h"
#include "World/BuilderAsset.h"
#include "World/Generation/MyGeometryFunctions.h"

TerrainType UTerrainBuilder::GetType(const FIntVector InPosition) const
{
	return MapDataRef->GetType(InPosition);
}

void UTerrainBuilder::ApplyNoiseToGeometry(const FIntVector InPosition)
{
	FGeometryScriptMeshSelection Selection;
	FGeometryScriptPerlinNoiseOptions Options;
	Options.BaseLayer.Magnitude = 30.f; // TODO magic number
	const FVector Offset = {static_cast<double>(FChunkData::GetChunkRealSize() * InPosition.X),
		static_cast<double>(FChunkData::GetChunkRealSize() * InPosition.Y),
		InPosition.Z * 300.f};
	UMyGeometryFunctions::ApplyPerlinNoiseToMeshAxis(ProcessedMesh, Selection, Options, Offset, true, true, false);
}

void UTerrainBuilder::Build(AWorldMap* InWorldMap, const FIntVector InPosition)
{
	FRWScopeLock ReadLock(InWorldMap->WorldLock, SLT_ReadOnly);
	MapDataRef = &InWorldMap->WorldMap;
	Position = InPosition;
	const FIntPoint Size = FChunkData::GetChunkFloorSize();
	const FIntVector ChunkOffset = {Position.X * Size.X, Position.Y * Size.Y, 0};

	int Index = 0; // TODO Add Material Index to BuildAsset
	for(TObjectPtr<UBuilderAsset> BuildAsset : CellsBuildDataAssets)
	{
		MeshBuffers.Add(FGridBuilder());
		for (int32 y = 0; y < Size.Y; y++)
		{
			for (int32 x = 0; x < Size.X; x++)
			{
				const FIntVector CellPosition = FIntVector{x,y,Position.Z} + ChunkOffset;
				BuildTerrainCell(CellPosition, BuildAsset, Index);
			}
		}
		
		FGeometryScriptIndexList OutList;
		UGeometryScriptLibrary_MeshBasicEditFunctions::AppendBuffersToMesh(ProcessedMesh, MeshBuffers[Index].GetBuffers(), OutList, Index);
		Index++;
	}
	
	ApplyNoiseToGeometry(Position);
}

bool UTerrainBuilder::HasOrRequirements(const FIntVector InPosition, const FOrRequirements& OrRequirements) const
{
	for(const FTypeAndPosition& TypeAndPosition : OrRequirements.OrRequirements)
	{
		const FIntVector TerrainPosition = InPosition + TypeAndPosition.Position;
		if(GetType(TerrainPosition) == TypeAndPosition.TerrainType)
		{
			return true;
		}
	}
	return false;
}

bool UTerrainBuilder::HasAndRequirements(const FIntVector InPosition, const FRequirements& Requirements) const
{
	for(const FOrRequirements& OrRequirements : Requirements.AndRequirements)
	{
		if(!HasOrRequirements(InPosition, OrRequirements))
		{
			return false;
		}
	}
	return true;
}

bool UTerrainBuilder::HasRequirements(const FIntVector InPosition, const FRequirements& Requirements) const
{
	return HasAndRequirements(InPosition, Requirements);
}

void UTerrainBuilder::BuildTerrainCell(const FIntVector InPosition, UBuilderAsset* BuildAsset, const int Index)
{
	for(const UCellBuildInfoAsset* CellInfo : BuildAsset->CellsBuildData)
	{
		if(HasRequirements(InPosition, CellInfo->Requirements))
		{
			const FIntVector InChunkPosition = FMapData::GetInChunkPosition(InPosition);
			const FTransform Transform =  FTransform(CellInfo->Transform.GetRotation(),GetRealPosition(FIntPoint{ InChunkPosition.X ,InChunkPosition.Y }) + CellInfo->Transform.GetLocation());
			CellInfo->Mesh->bAllowCPUAccess = true;
			
			MeshBuffers[Index].AddMesh(CellInfo->Buffers, Transform);
		}
	}
}

void UTerrainBuilder::Finish(UDynamicMeshComponent* MeshComponent)
{
	if(!MeshComponent) return;
	
	UDynamicMesh* OutMesh = MeshComponent->GetDynamicMesh();
	UGeometryScriptLibrary_MeshDecompositionFunctions::CopyMeshToMesh(ProcessedMesh, MeshComponent->GetDynamicMesh(), OutMesh);
	MeshPool->ReturnAllMeshes();
}

void UTerrainBuilder::Init()
{
	MeshBuffers.Reset();
	MeshPool = UGeometryScriptLibrary_SceneUtilityFunctions::CreateDynamicMeshPool();
	ProcessedMesh = MeshPool->RequestMesh();
	TemporaryHelpMesh = MeshPool->RequestMesh();
}

FVector UTerrainBuilder::GetRealPosition(const FIntPoint& GridPosition) const
{
	return FVector{ static_cast<float>(GridPosition.X) * GridCellLength + GridCellLength / 2.f,
					static_cast<float>(GridPosition.Y) * GridCellLength + GridCellLength / 2.f,
					0.f };
}

