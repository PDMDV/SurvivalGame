#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

UCLASS()
class MMO_API AMyAIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	AMyAIController();
protected:
	

	virtual void OnPossess(APawn* InPawn) override;
	
};
