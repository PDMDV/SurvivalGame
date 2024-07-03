
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldGeneratorBase.generated.h"

class UTextureRenderTarget2D;
class UStaticMesh;
struct FMapData;
struct FChunkData;

enum class TerrainType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class MMO_API UWorldGeneratorBase : public UObject
{
	GENERATED_BODY()

	friend class UMapGeneratorModifier;

public:	
	UWorldGeneratorBase();

	UPROPERTY(EditAnywhere)
	int32 LowestLevel;

	UPROPERTY(EditAnywhere)
	int32 HighestLevel;

	UPROPERTY(EditAnywhere)
	FIntPoint Size;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UMapGeneratorModifier>> Modifiers; // TODO use data table

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> ObjectsPositions;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	virtual void BuildMap(FMapData& Map, const FIntPoint& Position);
	void GenerateChunk(FChunkData* Chunk, const FIntPoint& Position);
	FIntPoint GetSize() const { return Size; };
	bool IsAnyObjectInRadius(const FVector& Center, float Radius);
};
