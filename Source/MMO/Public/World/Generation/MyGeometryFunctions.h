// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyGeometryFunctions.generated.h"

/**
 * 
 */
UCLASS()
class MMO_API UMyGeometryFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "GeometryScript|Deformations", meta=(ScriptMethod))
	static UPARAM(DisplayName = "Target Mesh") UDynamicMesh* ApplyPerlinNoiseToMeshAxis(  
		UDynamicMesh* TargetMesh, 
		FGeometryScriptMeshSelection Selection,
		FGeometryScriptPerlinNoiseOptions Options,
		FVector Offset,
		bool ApplyToX = true,
		bool ApplyToY = true,
		bool ApplyToZ = true,
		UGeometryScriptDebug* Debug = nullptr);
};
