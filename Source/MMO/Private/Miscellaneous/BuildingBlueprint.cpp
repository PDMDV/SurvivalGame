// Fill out your copyright notice in the Description page of Project Settings.


#include "Miscellaneous/BuildingBlueprint.h"

// Sets default values
ABuildingBlueprint::ABuildingBlueprint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABuildingBlueprint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuildingBlueprint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

