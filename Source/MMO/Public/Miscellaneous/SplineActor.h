#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "SplineActor.generated.h"

UCLASS()
class MMO_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<USplineComponent> SplineComponent;

public:
	ASplineActor();

	bool AddRandomPointToSpline(const FVector& InTargetLocation, float InRadius, float MaxRotation, float MinDistance);

	USplineComponent* GetSplineComponent() {return SplineComponent;};

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
