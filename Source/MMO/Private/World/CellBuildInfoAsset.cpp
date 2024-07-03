
#include "World/CellBuildInfoAsset.h"
#include "KismetProceduralMeshLibrary.h"

TArray<FIntVector> UCellBuildInfoAsset::ConvertFromIntsToIntVectors(TArray<int32> InArray)
{
	TArray<FIntVector> Result;
	for(int i = 0 ; i < InArray.Num(); i+=3)
	{
		Result.Push({InArray[i],InArray[i+1], InArray[i+2]});
	}
	return Result;
}

void UCellBuildInfoAsset::Build()
{
	TArray<int32> Triangles;
	TArray<FProcMeshTangent> Tangents;
	
	UKismetProceduralMeshLibrary::GetSectionFromStaticMesh
	(
		Mesh,
		0,
		0,
		Buffers.Vertices,
		Triangles,
		Buffers.Normals,
		Buffers.UV0,
		Tangents
	);
	Buffers.Triangles = ConvertFromIntsToIntVectors(Triangles);
}
