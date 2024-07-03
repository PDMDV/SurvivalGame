
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataLayerHook.generated.h"

class AChunk;

UCLASS()
class MMO_API ADataLayerHook : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	bool bIsFree = true;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorClass;
	
public:	
	ADataLayerHook();
	bool IsFree() const;
	void SetIsFree(bool bInIsFree);
	
protected:
	virtual void BeginPlay() override;
};
