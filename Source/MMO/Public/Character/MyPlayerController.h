
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "ControllingInput.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "MyPlayerController.generated.h"

class UAbilityAsync_WaitGameplayEvent;
class UTagMap;
class UInputEvents;
class UInputToTagMapping;
class UInputAction;
class UInputMappingContext;

UCLASS()
class AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	UPROPERTY(VisibleAnywhere)
	bool bCameraCanRotate = false;

	UPROPERTY(VisibleAnywhere)
	FVector2D LastMousePosition;

	UPROPERTY(EditAnywhere)
	float MouseScrollFactor = 0.1f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FRotator TargetRotation;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly)
	float RelativeToMovementYawRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UControllingInput> InputAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputEvents> InputEvents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UTagMap> InputToAbilityBinding;

protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void AcknowledgePossession(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	
public:

	UFUNCTION(Server, Unreliable)
	void ReplicateTargetRotationToServer(FRotator NewTargetRotation);

	UFUNCTION(Server, Reliable)
	void SendGameplayEventToServer(FGameplayTag Tag);

	void ActiveMappingContext();
	void BindInputActions(UEnhancedInputComponent* EnhancedInputComponent);
	void SendGameplayEvent(FGameplayTag Tag, bool bReplicateToServer = false);
	void ActiveAbility(const FInputActionValue& InValue, FGameplayTag Tag);
	void HandleGameplayEvent(FGameplayTag MatchingTag, const FGameplayEventData* Payload);
	void MoveControlledPawn(const FInputActionValue& InValue);
	void MoveCamera(const FInputActionValue& InValue);
	void EnableCameraRotating(const FInputActionValue& InValue);
	void RotateCameraYaw(const FInputActionValue& InValue);
	void ChangeCameraDistance(const FInputActionValue& InValue);
	void CalculateTargetRotation();
	
	FVector2D GetPlayersBottomScreenPosition() const;
	float GetRelativeToMovementYawRotation() const;
};


