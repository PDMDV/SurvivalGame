
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class UAttributeBarsData;
class UAbilitySystemComponent;
class UOverlayWidget;

UCLASS()
class MMO_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidget> OverlayWidget;

	UFUNCTION(BlueprintPure)
	UOverlayWidget* GetOverlay() { return OverlayWidget; };

	void Initialize();
	
protected:
	virtual void BeginPlay() override;
};
