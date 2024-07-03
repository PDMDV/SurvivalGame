// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GeometryScript/MeshBasicEditFunctions.h"
#include "CellBuildInfoAsset.generated.h"

enum class TerrainType : uint8;

USTRUCT()
struct FTypeAndPosition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TerrainType TerrainType;

	UPROPERTY(EditDefaultsOnly)
	FIntVector Position;
};

USTRUCT()
struct FOrRequirements
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FTypeAndPosition> OrRequirements;
};

USTRUCT()
struct FRequirements
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FOrRequirements> AndRequirements;
};

UCLASS(BlueprintType)
class MMO_API UCellBuildInfoAsset : public UDataAsset
{
	GENERATED_BODY()

	TArray<FIntVector> ConvertFromIntsToIntVectors(TArray<int32> InArray);
	
public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY()
	FGeometryScriptSimpleMeshBuffers Buffers;

	UPROPERTY(EditDefaultsOnly)
	FTransform Transform;

	UPROPERTY(EditDefaultsOnly)
	FRequirements Requirements;
	
	UFUNCTION(CallInEditor)
	void Build();
};
