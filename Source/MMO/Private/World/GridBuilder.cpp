
#include "World/GridBuilder.h"
#include "KismetProceduralMeshLibrary.h"
#include "GeometryScript/MeshBasicEditFunctions.h"

void FGridBuilder::ApplyTransform(FGeometryScriptSimpleMeshBuffers& InData, const FTransform& Transform)
{
	for (FVector& Vertex : InData.Vertices)
	{
		Vertex = Transform.GetRotation().RotateVector(Vertex);
		Vertex = Vertex + Transform.GetLocation();
	}
	for (FVector& Normal : InData.Normals)
	{
		Normal = Transform.GetRotation().RotateVector(Normal);
	}
}

void FGridBuilder::AddMesh(FGeometryScriptSimpleMeshBuffers MeshData, const FTransform& Transform)
{
	ApplyTransform(MeshData, Transform);
	MergeData(MeshData);
}

void FGridBuilder::MergeData(const FGeometryScriptSimpleMeshBuffers& InData)
{
	const int LastIndex = Data.Vertices.Num();
	Data.Vertices.Append(InData.Vertices);
	Data.Normals.Append(InData.Normals);
	Data.UV0.Append(InData.UV0);
	
	for (const FIntVector Triangle: InData.Triangles)
	{
		FIntVector NewTriangle = Triangle + FIntVector{LastIndex,LastIndex,LastIndex};
		Data.Triangles.Push(NewTriangle);
	}
}

void FGridBuilder::Reset()
{
	Data = FGeometryScriptSimpleMeshBuffers();
}

TArray<FIntVector> FGridBuilder::ConvertFromIntsToIntVectors(TArray<int32> InArray)
{
	TArray<FIntVector> Result;
	for(int i = 0 ; i < InArray.Num(); i+=3)
	{
		Result.Push({InArray[i],InArray[i+1], InArray[i+2]});
	}
	return Result;
}

FGeometryScriptSimpleMeshBuffers FGridBuilder::GetBuffers() const
{
	return Data;
}
