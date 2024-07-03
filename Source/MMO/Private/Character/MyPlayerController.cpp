		
#include "Character/MyPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/PlayerCharacter.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/InputEvents.h"
#include "Character/TagMap.h"
#include "Game/MyGameLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UI/MyHUD.h"

AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	bEnableClickEvents = true;
	bReplicates = true;
}

void AMyPlayerController::ActiveMappingContext()
{
	if(IsLocalController())
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		InputSubsystem->AddMappingContext(MappingContext, 0);
	}
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ActiveMappingContext();
}

void AMyPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(!GetPawn()) return;
	
	if (IsLocalController())
	{
		CalculateTargetRotation();
	}
}

void AMyPlayerController::SendGameplayEventToServer_Implementation(FGameplayTag Tag)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), Tag, FGameplayEventData());
}

void AMyPlayerController::BindInputActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if(!InputEvents) return;
	
	for(const FInputTags& InputAction : InputEvents->InputTags)
	{
		for(const TTuple<ETriggerEvent, FGameplayTag>& Event : InputAction.Events)
		{
			const ETriggerEvent TriggerEvent = Event.Key;
			const FGameplayTag Tag = Event.Value;
			EnhancedInputComponent->BindAction(InputAction.Input, TriggerEvent, this, &ThisClass::SendGameplayEvent, Tag, true);
		}
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(InputAsset->Keys_WSAD, ETriggerEvent::Triggered, this, &ThisClass::MoveControlledPawn);
	EnhancedInputComponent->BindAction(InputAsset->MousePosition, ETriggerEvent::Triggered, this, &ThisClass::MoveCamera);
	EnhancedInputComponent->BindAction(InputAsset->MouseMiddleClicked, ETriggerEvent::Triggered, this, &ThisClass::EnableCameraRotating);
	EnhancedInputComponent->BindAction(InputAsset->Keys_QE, ETriggerEvent::Triggered, this, &ThisClass::RotateCameraYaw);
	EnhancedInputComponent->BindAction(InputAsset->MouseScroll, ETriggerEvent::Triggered, this, &ThisClass::ChangeCameraDistance);
	
	BindInputActions(EnhancedInputComponent);
}

void AMyPlayerController::SendGameplayEvent(const FGameplayTag Tag, bool bReplicateToServer)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), Tag, FGameplayEventData());
	if(!HasAuthority() && bReplicateToServer)
	{
		SendGameplayEventToServer(Tag);
	}
}

void AMyPlayerController::ActiveAbility(const FInputActionValue& InValue, FGameplayTag Tag)
{
	if(const IAbilitySystemInterface* PlayerInterface = Cast<IAbilitySystemInterface>(GetPawn()))
	{
		UAbilitySystemComponent* AbilitySystemComponent = PlayerInterface->GetAbilitySystemComponent();
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(Tag));
	}
}

void AMyPlayerController::MoveControlledPawn(const FInputActionValue& InValue)
{
	FVector Direction = FVector(InValue.Get<FVector2D>(),0.f);
	if(ABaseCharacter* ControlledCharacter = Cast<ABaseCharacter>(GetPawn()))
	{
		FRotator Rotation = GetControlRotation();
		Rotation.Pitch = 0.f;
		Direction = Rotation.RotateVector(Direction);
		ControlledCharacter->AddMovementInput(Direction);
	}
}

void AMyPlayerController::MoveCamera(const FInputActionValue& InValue)
{
	const FVector Direction = FVector(InValue.Get<FVector2D>(),0.f);
	if (bCameraCanRotate)
	{
		AddYawInput(Direction.X);
		AddPitchInput(Direction.Y);
		SetMouseLocation(LastMousePosition.X, LastMousePosition.Y);
	}
}

void AMyPlayerController::EnableCameraRotating(const FInputActionValue& InValue)
{
	bCameraCanRotate = InValue.Get<bool>();
	SetShowMouseCursor(!bCameraCanRotate);
	if (bCameraCanRotate)
	{
		GetMousePosition(LastMousePosition.X, LastMousePosition.Y);
	}
}

void AMyPlayerController::RotateCameraYaw(const FInputActionValue& InValue)
{
	AddYawInput(InValue.Get<float>());
}

void AMyPlayerController::ChangeCameraDistance(const FInputActionValue& InValue)
{
	const float Value = InValue.Get<float>();
	if(APlayerCharacter* ControlledPlayer = Cast<APlayerCharacter>(GetPawn()))
	{
		ControlledPlayer->MultiplyCameraDistance(1.f + (Value * MouseScrollFactor));
	}
}

FVector2D AMyPlayerController::GetPlayersBottomScreenPosition() const
{
	FVector2D Result;
	const FBox BoundingBox = GetPawn()->GetComponentsBoundingBox();
	const float HalfOfHeight = BoundingBox.GetSize().Z / 2.0f;
	ProjectWorldLocationToScreen(GetPawn()->GetActorLocation() - FVector(0,0,HalfOfHeight), Result);
	return Result;
}

void AMyPlayerController::CalculateTargetRotation()
{
	check(IsLocalController())
	
	FVector2D MouseScreenPosition;
	if(GetMousePosition(MouseScreenPosition.X, MouseScreenPosition.Y))
	{
		const FVector2D PlayersBottomScreenPosition = GetPlayersBottomScreenPosition();
	
		TargetRotation = UKismetMathLibrary::FindLookAtRotation(FVector(PlayersBottomScreenPosition - MouseScreenPosition, 0.0f), FVector(0.0f));
		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.f, GetControlRotation().Yaw + 90.f, 0.f));
		if(APlayerCharacter* ControlledPlayer = Cast<APlayerCharacter>(GetPawn()))
		{
			ControlledPlayer->SetDesireRotation(TargetRotation);
			ControlledPlayer->FaceRotation(TargetRotation);
		}
		ReplicateTargetRotationToServer(TargetRotation);
	}
}

float AMyPlayerController::GetRelativeToMovementYawRotation() const
{
	return RelativeToMovementYawRotation;
}

void AMyPlayerController::ReplicateTargetRotationToServer_Implementation(const FRotator NewTargetRotation)
{
	TargetRotation = NewTargetRotation;
	if(APlayerCharacter* ControlledPlayer = Cast<APlayerCharacter>(GetPawn()))
	{
		ControlledPlayer->SetDesireRotation(TargetRotation);
	}
}

void AMyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerController, TargetRotation)
}

void AMyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AMyPlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);

	if(AMyHUD* HUD = Cast<AMyHUD>(GetHUD()))
	{
		HUD->Initialize();
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(), FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &AMyPlayerController::HandleGameplayEvent));
}

void AMyPlayerController::HandleGameplayEvent(FGameplayTag MatchingTag, const FGameplayEventData* Payload)
{
	if(FGameplayTag* Tag = InputToAbilityBinding->BindingMap.Find(MatchingTag))
	{
		SendGameplayEvent(*Tag);
	}
}
