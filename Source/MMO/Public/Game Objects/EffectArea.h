
#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameplayEffect.h"
#include "EffectArea.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class MMO_API UEffectArea : public UBoxComponent
{
	GENERATED_BODY()

public:
	UEffectArea();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> Effect;

	UPROPERTY(VisibleAnywhere)
	TMap<UAbilitySystemComponent*, FActiveGameplayEffectHandle> ActiveEffectHandles;
	
	UFUNCTION()
	void StartOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
	void StopOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	void RemoveAllEffects();
};

