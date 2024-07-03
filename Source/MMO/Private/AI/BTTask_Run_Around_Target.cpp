
#include "AI/BTTask_Run_Around_Target.h"
#include "VisualLogger/VisualLogger.h"
#include "AIController.h"
#include "Miscellaneous/SplineActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Tasks/AITask_MoveTo.h"

UBTTask_Run_Around_Target::UBTTask_Run_Around_Target()
{
	NodeName = "Run Around Target";

	bObserveBlackboardValue = true;

	INIT_TASK_NODE_NOTIFY_FLAGS();
	
	// accept only actors and vectors
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Run_Around_Target, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Run_Around_Target, BlackboardKey));
}

uint16 UBTTask_Run_Around_Target::GetInstanceMemorySize() const
{
	return sizeof(FBTMRunAroundTargetMemory);
}

void UBTTask_Run_Around_Target::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	FBTMRunAroundTargetMemory* MyMemory = CastInstanceNodeMemory<FBTMRunAroundTargetMemory>(NodeMemory);
	MyMemory->Task.Reset();

	if (bObserveBlackboardValue)
	{
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (ensure(BlackboardComp) && MyMemory->BBObserverDelegateHandle.IsValid())
		{
			BlackboardComp->UnregisterObserver(BlackboardKey.GetSelectedKeyID(), MyMemory->BBObserverDelegateHandle);
		}

		MyMemory->BBObserverDelegateHandle.Reset();
	}

	if(MyMemory->SplineActor)
	{
		MyMemory->SplineActor->Destroy();
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_Run_Around_Target::MoveToNextPoint(UBehaviorTreeComponent* BehaviorComp)
{
	uint8* RawMemory = BehaviorComp->GetNodeMemory(this, BehaviorComp->FindInstanceContainingNode(this));
	FBTMRunAroundTargetMemory* MyMemory = CastInstanceNodeMemory<FBTMRunAroundTargetMemory>(RawMemory);
	
	if( MyMemory->UpdateDistance < MyMemory->SplineStepIndex * MoveAccuracy)
	{
		bool bSuccess = MyMemory->SplineActor->AddRandomPointToSpline(MyMemory->TargetLocation, Radius, MaxRotation, MinDistanceToNextPoint);
		RecalculateUpdateDistance(MyMemory);
		if(!bSuccess)
		{
			FinishLatentTask(*BehaviorComp, EBTNodeResult::Failed );
		}
	}
	const USplineComponent* SplineComponent = MyMemory->SplineActor->GetSplineComponent();
	const FVector NextTarget = SplineComponent->GetLocationAtDistanceAlongSpline(MyMemory->SplineStepIndex * MoveAccuracy, ESplineCoordinateSpace::World);
		
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(NextTarget);
		
	UAITask_MoveTo* MoveTask = NewBTAITask<UAITask_MoveTo>(*BehaviorComp);
	if (MoveTask) 
	{
		MoveTask->SetUp(MoveTask->GetAIController(), MoveRequest);
		MyMemory->Task = MoveTask;
		MyMemory->SplineStepIndex++;
		MoveTask->ReadyForActivation();
	}

}

void UBTTask_Run_Around_Target::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	UAITask_MoveTo* MoveTask = Cast<UAITask_MoveTo>(&Task);
	if (MoveTask && MoveTask->GetAIController() && MoveTask->GetState() != EGameplayTaskState::Paused)
	{
		UBehaviorTreeComponent* BehaviorComp = GetBTComponentForTask(Task);
		if (BehaviorComp)
		{
			MoveToNextPoint(BehaviorComp);
		}
	}
}

void UBTTask_Run_Around_Target::RecalculateUpdateDistance(FBTMRunAroundTargetMemory* MyMemory)
{
	const USplineComponent* SplineComponent = MyMemory->SplineActor->GetSplineComponent();
	const int32 PenultimatePointIndex = SplineComponent->GetNumberOfSplinePoints() - 2;
	MyMemory->UpdateDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(PenultimatePointIndex);
}

EBTNodeResult::Type UBTTask_Run_Around_Target::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();

	FBTMRunAroundTargetMemory* MyMemory = CastInstanceNodeMemory<FBTMRunAroundTargetMemory>(NodeMemory);
	MyMemory->Controller = MyController;
	
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	
	if (MyController == nullptr)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UBTTask_Run_Around_Target::ExecuteTask failed since AIController is missing."));
		return EBTNodeResult::Failed;
	}

	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		MyMemory->TargetActor = Cast<AActor>(KeyValue);
		if(MyMemory->TargetActor)
		{
			MyMemory->TargetLocation = MyMemory->TargetActor->GetActorLocation();
		}
		else
		{
			UE_VLOG(MyController, LogBehaviorTree, Warning, TEXT("UBTTask_Run_Around_Target::ExecuteTask tried to go to actor while BB %s entry was empty"), *BlackboardKey.SelectedKeyName.ToString());
			return EBTNodeResult::Failed;
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		MyMemory->TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	}
	
	const APawn* ControlledPawn = MyController->GetPawn();
	MyMemory->SplineActor = GetWorld()->SpawnActor<ASplineActor>(SplineActorClass, ControlledPawn->GetTransform());
	MyMemory->SplineStepIndex = 0;

	for(int32 i = 0 ; i < ForwardSplinePointsNum; i++)
	{
		bool Success = MyMemory->SplineActor->AddRandomPointToSpline(MyMemory->TargetLocation, Radius, MaxRotation, MinDistanceToNextPoint);
		if(!Success)
		{
			return EBTNodeResult::Failed;
		}
	}

	if (NodeResult == EBTNodeResult::InProgress && bObserveBlackboardValue)
	{
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (ensure(BlackboardComp))
		{
			if (MyMemory->BBObserverDelegateHandle.IsValid())
			{
				UE_VLOG(MyController, LogBehaviorTree, Warning, TEXT("UBTTask_MoveAroundTarget::ExecuteTask \'%s\' Old BBObserverDelegateHandle is still valid! Removing old Observer."), *GetNodeName());
				BlackboardComp->UnregisterObserver(BlackboardKey.GetSelectedKeyID(), MyMemory->BBObserverDelegateHandle);
			}
			MyMemory->BBObserverDelegateHandle = BlackboardComp->RegisterObserver(BlackboardKey.GetSelectedKeyID(), this, FOnBlackboardChangeNotification::CreateUObject(this, &UBTTask_Run_Around_Target::OnBlackboardValueChange));
		}
	}
	
	RecalculateUpdateDistance(MyMemory);
	MoveToNextPoint(&OwnerComp);
	
	return EBTNodeResult::InProgress;
}

EBlackboardNotificationResult UBTTask_Run_Around_Target::OnBlackboardValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = Cast<UBehaviorTreeComponent>(Blackboard.GetBrainComponent());
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	const AAIController* MyController = BehaviorComp->GetAIOwner();
	uint8* RawMemory = BehaviorComp->GetNodeMemory(this, BehaviorComp->FindInstanceContainingNode(this));
	FBTMRunAroundTargetMemory* MyMemory = CastInstanceNodeMemory<FBTMRunAroundTargetMemory>(RawMemory);

	const EBTTaskStatus::Type TaskStatus = BehaviorComp->GetTaskStatus(this);
	if (TaskStatus != EBTTaskStatus::Active)
	{
		UE_VLOG(MyController, LogBehaviorTree, Error, TEXT("BT Run Around Target \'%s\' task observing BB entry while no longer being active!"), *GetNodeName());
		MyMemory->BBObserverDelegateHandle.Reset();
		return EBlackboardNotificationResult::RemoveObserver;
	}
	
	// this means the move has already started. 
	if (MyMemory != nullptr && BehaviorComp->GetAIOwner() != nullptr)
	{
		check(BehaviorComp->GetAIOwner()->GetPathFollowingComponent());

		bool bUpdateMove = true;
		// check if new goal is almost identical to previous one
		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			const FVector NewTargetLocation = Blackboard.GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
			MyMemory->TargetLocation = NewTargetLocation;
		}
		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = Blackboard.GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			AActor* NewTargetActor = Cast<AActor>(KeyValue);
			if(NewTargetActor != MyMemory->TargetActor)
			{
				MyMemory->TargetActor = NewTargetActor;
				MyMemory->TargetLocation = NewTargetActor->GetActorLocation();
			}
		}
	}

	return EBlackboardNotificationResult::ContinueObserving;
}

EBTNodeResult::Type UBTTask_Run_Around_Target::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTMRunAroundTargetMemory* MyMemory = CastInstanceNodeMemory<FBTMRunAroundTargetMemory>(NodeMemory);

	UAITask_MoveTo* MoveTask = MyMemory->Task.Get();
	if (MoveTask)
	{
		MoveTask->ExternalCancel();
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}

FString UBTTask_Run_Around_Target::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() ||
		BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}

void UBTTask_Run_Around_Target::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (BlackboardComp)
	{
		const FString KeyValue = BlackboardComp->DescribeKeyValue(BlackboardKey.GetSelectedKeyID(), EBlackboardDescription::OnlyValue);

		const FBTMRunAroundTargetMemory* MyMemory = CastInstanceNodeMemory<FBTMRunAroundTargetMemory>(NodeMemory);
		const bool bIsUsingTask = MyMemory->Task.IsValid();
		
		const FString ModeDesc = bIsUsingTask ? TEXT("(task)") : TEXT("");

		Values.Add(FString::Printf(TEXT("move target: %s%s"), *KeyValue, *ModeDesc));
	}
}

void UBTTask_Run_Around_Target::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTMRunAroundTargetMemory* MyMemory = CastInstanceNodeMemory<FBTMRunAroundTargetMemory>(NodeMemory);
	if(MyMemory->TargetActor && bObserveBlackboardValue)
	{
		MyMemory->TargetLocation = MyMemory->TargetActor->GetActorLocation();
	}
}
