#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverlayWidget.generated.h"

class UAbilitiesWidget;
class UCharacterAttributeBars;

UCLASS()
class MMO_API UOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterAttributeBars> CharacterStateBars;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilitiesWidget> AbilitiesPanel;
};
