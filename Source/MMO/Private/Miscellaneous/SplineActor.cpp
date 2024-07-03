
#include "Miscellaneous/SplineActor.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"

ASplineActor::ASplineActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SetRootComponent(SplineComponent);
}

bool ASplineActor::AddRandomPointToSpline(const FVector& InTargetLocation, float InRadius, float MaxRotation, float MinDistance)
{
	check(InRadius > MinDistance)

	const int32 MaxAttemptsNum = 100;
	
	FVector NewPointPosition;
	FRotator BetweenRotator;
	int32 AttemptsNum = 0;
	float Distance;
	bool bExceedsMaximumRotation;
	bool bIsNextPointFarEnoughFromLastPoint;
	do {
		const bool Success = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, InTargetLocation, NewPointPosition, InRadius);

		if( AttemptsNum > MaxAttemptsNum || !Success) return false;
		AttemptsNum++;
		
		const int32 LastPointIndex = GetSplineComponent()->GetNumberOfSplinePoints() - 1;
		const FVector LastPointPosition = GetSplineComponent()->GetLocationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);
		const FRotator LastDirection =  GetSplineComponent()->GetRotationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);
		const FRotator NewPointRotator = UKismetMathLibrary::FindLookAtRotation(LastPointPosition, NewPointPosition);
		BetweenRotator = FQuat::FindBetween(LastDirection.Vector(),NewPointRotator.Vector()).Rotator();
		
		Distance = FVector::Distance(NewPointPosition, LastPointPosition);
		bIsNextPointFarEnoughFromLastPoint = Distance > MinDistance;
		bExceedsMaximumRotation = FMath::Abs(BetweenRotator.Yaw) > MaxRotation;
	} while( bExceedsMaximumRotation || !bIsNextPointFarEnoughFromLastPoint );
	
	GetSplineComponent()->AddSplinePoint(NewPointPosition , ESplineCoordinateSpace::World, true);
	return true;
}

void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

