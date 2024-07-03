// Fill out your copyright notice in the Description page of Project Settings.


#include "UI\MyHUD.h"

#include "UI\OverlayWidget.h"
#include "Blueprint/UserWidget.h"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AMyHUD::Initialize()
{
	if(IsValid(OverlayWidgetClass))
	{
		OverlayWidget = CreateWidget<UOverlayWidget>(GetWorld(), OverlayWidgetClass);
		OverlayWidget->AddToViewport();
	}
}
