
#pragma once

#include "BarAsset.h"
#include "Engine/DataAsset.h"
#include "AttributeBarsData.generated.h"

UCLASS(BlueprintType, Blueprintable)
class MMO_API UAttributeBarsData : public UDataAsset
{
 	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BarsData")
	TArray<TObjectPtr<UBarAsset>> BarsAssets;
};
