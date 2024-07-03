
#pragma once

#include "CoreMinimal.h"
#include "WorldGeneratorBase.h"
#include "GeneratorTerrain.generated.h"

UCLASS()
class MMO_API UGeneratorTerrain : public UWorldGeneratorBase
{
	GENERATED_BODY()

public:

	virtual void BuildMap(FMapData& Map, const FIntPoint& Position) override;
};
