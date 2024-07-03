
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingDataAsset.generated.h"

class AGameObject;

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	Wood	UMETA(DisplayName = "Wood"),
	Stone	UMETA(DisplayName = "Stone"),
	Iron	UMETA(DisplayName = "Iron"),
};

UCLASS(BlueprintType)
class MMO_API UBuildingDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGameObject> BuildingClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> BlueprintMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool SnapToGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EResourceType, int32> RequiredResource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> BlockingPlacementClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> IgnoringPlacementClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> RequiredPlacementClasses;
	
};
