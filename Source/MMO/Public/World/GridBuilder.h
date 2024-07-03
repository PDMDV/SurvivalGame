
#pragma once

#include "CoreMinimal.h"
#include "GeometryScript/MeshBasicEditFunctions.h"
#include "GridBuilder.generated.h"

class UDynamicMeshComponent;
struct FGeometryScriptSimpleMeshBuffers;

USTRUCT()
struct FGridBuilder
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	TMap<UStaticMesh*, FGeometryScriptSimpleMeshBuffers> MeshMap;

	UPROPERTY()
	FGeometryScriptSimpleMeshBuffers Data;
	
	static TArray<FIntVector> ConvertFromIntsToIntVectors(TArray<int32> InArray);

	void ApplyTransform(FGeometryScriptSimpleMeshBuffers& Data, const FTransform& Transform);
	void AddMesh(FGeometryScriptSimpleMeshBuffers MeshData, const FTransform& Transform);
	void MergeData(const FGeometryScriptSimpleMeshBuffers& InData);
	void Reset();
	
	bool bMergeVerticals = false;
	FGeometryScriptSimpleMeshBuffers GetBuffers() const;
};
