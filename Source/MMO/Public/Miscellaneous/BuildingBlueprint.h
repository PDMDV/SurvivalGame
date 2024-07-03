#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingBlueprint.generated.h"

UCLASS()
class MMO_API ABuildingBlueprint : public AActor
{
	GENERATED_BODY()
	
public:	
	ABuildingBlueprint();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
