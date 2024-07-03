// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NonPlayerCharacter.h"

#include "Character/MyAIController.h"

ANonPlayerCharacter::ANonPlayerCharacter()
{
}

void ANonPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}
