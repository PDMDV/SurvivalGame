// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "NonPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MMO_API ANonPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	ANonPlayerCharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;
};
