// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Generation/MyGeometryFunctions.h"
#include "GeometryScript/MeshDeformFunctions.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "EngineDefines.h"
#include "Async/ParallelFor.h"

#include "UDynamicMesh.h"

UDynamicMesh* UMyGeometryFunctions::ApplyPerlinNoiseToMeshAxis(
	UDynamicMesh* TargetMesh,
	FGeometryScriptMeshSelection Selection,
	FGeometryScriptPerlinNoiseOptions Options,
	FVector Offset,
	bool ApplyToX,
	bool ApplyToY,
	bool ApplyToZ,
	UGeometryScriptDebug* Debug)
{
	if (TargetMesh == nullptr)
	{
		return TargetMesh;
	}
	if (Selection.IsEmpty() && Options.EmptyBehavior != EGeometryScriptEmptySelectionBehavior::FullMeshSelection )
	{
		return TargetMesh;
	}

	TargetMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& EditMesh) 
	{
		const FRandomStream RandomStream(Options.BaseLayer.RandomSeed);
		FVector3d Offsets[3];
		for (int k = 0; k < 3; ++k)
		{
			const float RandomOffset = 10000.0f * RandomStream.GetFraction();
			Offsets[k] = FVector3d(RandomOffset, RandomOffset, RandomOffset);
			Offsets[k] += (FVector3d)Options.BaseLayer.FrequencyShift;
		}
		
		auto GetDisplacedPosition = [&EditMesh, &Options, &Offsets, Offset, ApplyToX, ApplyToY, ApplyToZ](int32 VertexID)
		{
			FVector3d Pos = EditMesh.GetVertex(VertexID);
			FVector3d Displacement;
			for (int32 k = 0; k < 3; ++k)
			{
				FVector NoisePos = (FVector)((double)Options.BaseLayer.Frequency * (Pos + Offsets[k] + Offset));
				Displacement[k] = Options.BaseLayer.Magnitude * FMath::PerlinNoise3D(Options.BaseLayer.Frequency * NoisePos);
			}
			Pos += FVector3d(ApplyToX ? Displacement.X : 0.0f, ApplyToY ? Displacement.Y : 0.0f, ApplyToZ ? Displacement.Z : 0.0f);
			return Pos;
		};

		if (Selection.IsEmpty())
		{
			ParallelFor(EditMesh.MaxVertexID(), [&](int32 VertexID)
			{
				if (EditMesh.IsVertex(VertexID))
				{
					EditMesh.SetVertex(VertexID, GetDisplacedPosition(VertexID));
				}
			});
		}
		else
		{
			Selection.ProcessByVertexID(EditMesh, [&](int32 VertexID)
			{
				EditMesh.SetVertex(VertexID, GetDisplacedPosition(VertexID));
			});
		}

	}, EDynamicMeshChangeType::GeneralEdit, EDynamicMeshAttributeChangeFlags::Unknown, false);

	return TargetMesh;
}
