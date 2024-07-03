
#include "Game Objects/GameObject.h"

#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "World/Terrain.h"

AGameObject::AGameObject()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	RootComponent = Mesh;
	
	BoundBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Drop Box"));
	BoundBox->SetupAttachment(RootComponent);
	BoundBox->SetGenerateOverlapEvents(false);
	BoundBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoundBox->SetGenerateOverlapEvents(true);
	BoundBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	bReplicates = true;
}

FVector AGameObject::GetBoundBoxSize(TSubclassOf<AGameObject> Class)
{
	if(!IsValid(Class)) return FVector();
	const AGameObject* DefaultObject = Class.GetDefaultObject();
	return DefaultObject->BoundBox->GetUnscaledBoxExtent();
}

FVector AGameObject::GetBoundBoxLocation(TSubclassOf<AGameObject> Class)
{
	if(!IsValid(Class)) return FVector();
	const AGameObject* DefaultObject = Class.GetDefaultObject();
	return DefaultObject->BoundBox->GetRelativeLocation();
}

void AGameObject::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(GetOwner()))
	{
		Cast<ATerrain>(GetOwner())->AddObject(this);
	}
}

void AGameObject::Destroyed()
{
	Super::Destroyed();
}

float AGameObject::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth -= FMath::Clamp(DamageAmount, 0.0f, MaxHealth );
	if (CurrentHealth <= 0.0f)
	{
		DestroyObject();
	}
	return DamageAmount;
}

void AGameObject::DestroyObject()
{
	if (GetWorld()->IsGameWorld() && HasAuthority())
	{
		if (SuccessorClass)
		{
			FTransform Transform(GetActorTransform());
			Transform.SetScale3D(FVector(1,1,1));
			GetWorld()->SpawnActor<AGameObject>(SuccessorClass, Transform);
		}
		
		for (TSubclassOf<AItem> DroppingItemClass : DroppingItems)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			FVector DropLocation = GetActorLocation() + UKismetMathLibrary::RandomPointInBoundingBox(BoundBox->GetRelativeLocation(), BoundBox->GetScaledBoxExtent());
			FRotator DropRotation(0,0,90);
			GetWorld()->SpawnActor<AItem>(DroppingItemClass, FTransform(DropRotation, DropLocation,{1,1,1}), Params);
		}
	}
	Destroy();
}
