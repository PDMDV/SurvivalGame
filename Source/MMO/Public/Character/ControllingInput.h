
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ControllingInput.generated.h"

class UInputAction;

UCLASS(BlueprintType)
class MMO_API UControllingInput : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Keyboard")
	TObjectPtr<UInputAction> Keys_WSAD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Keyboard")
	TObjectPtr<UInputAction> Keys_QE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Keyboard")
	TObjectPtr<UInputAction> Key_F;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mouse")
	TObjectPtr<UInputAction> MousePosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mouse")
	TObjectPtr<UInputAction> MouseRightClicked;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mouse")
	TObjectPtr<UInputAction> MouseMiddleClicked;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mouse")
	TObjectPtr<UInputAction> MouseLeftClicked;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mouse")
	TObjectPtr<UInputAction> MouseScroll;
};
