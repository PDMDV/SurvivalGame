#pragma once

#include "Character/BaseCharacter.h"
#include "CoreMinimal.h"
#include "PlayerCharacter.generated.h"

class ANavMeshBoundsVolume;
struct FActiveGameplayEffectHandle;

class USpringArmComponent;
class AWorldMap;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAbilitySysComp;
class UCharacterAttributeSet;
class UEffectsComponent;
class UCombatComponent;
class AItem;
class UCameraComponent;

UCLASS()
class MMO_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<AWorldMap> WorldMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	FRotator DesireRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<USceneComponent> GroundPosition;

	bool bIsUnderground = false; // TODO should be a tag

	UFUNCTION(BlueprintCallable)
	void SetDesireRotation(FRotator InDesireRotation);

	void MultiplyCameraDistance(float Distance);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};