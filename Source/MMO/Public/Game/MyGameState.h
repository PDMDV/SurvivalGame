// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

class AWorldMap;
UCLASS()
class MMO_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AWorldMap> WorldMap;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> WorldMapClass;

	UFUNCTION(BlueprintPure)
	AWorldMap* GetWorldMap() const {return WorldMap;};

private:
	void FindWorldMap();

protected:
	virtual void BeginPlay() override;
};
