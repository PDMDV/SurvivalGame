#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Miscellaneous/SplineActor.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Run_Around_Target.generated.h"

class UAITask_MoveTo;

struct FBTMRunAroundTargetMemory
{
	AActor* TargetActor;
	AAIController* Controller;
	ASplineActor* SplineActor;
	
	int32 SplineStepIndex;
	FVector TargetLocation;
	float UpdateDistance;
	
	TWeakObjectPtr<UAITask_MoveTo> Task;
	FDelegateHandle BBObserverDelegateHandle;
};

UCLASS()
class MMO_API UBTTask_Run_Around_Target : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_Run_Around_Target();

	
	UPROPERTY(Category = Blackboard, EditAnywhere)
	uint32 bObserveBlackboardValue : 1;

	UPROPERTY(EditAnywhere)
	float Radius = 1000.f;

	UPROPERTY(EditAnywhere)
	float MoveAccuracy = 100.f;

	UPROPERTY(EditAnywhere)
	float MaxRotation = 145.f;

	UPROPERTY(EditAnywhere)
	float MinDistanceToNextPoint = 300.f;

	UPROPERTY()
	TSubclassOf<ASplineActor> SplineActorClass = ASplineActor::StaticClass();

	UPROPERTY()
	int32 ForwardSplinePointsNum = 2;

protected:
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;

	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void MoveToNextPoint(UBehaviorTreeComponent* BehaviorComp);
	void RecalculateUpdateDistance(FBTMRunAroundTargetMemory* MyMemory);
	EBlackboardNotificationResult OnBlackboardValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID);
};
