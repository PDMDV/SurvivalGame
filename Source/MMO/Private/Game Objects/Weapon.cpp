
#include "Game Objects/Weapon.h"

#include "Components/CapsuleComponent.h"

AWeapon::AWeapon()
{
	DamageArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Damage Area"));
	DamageArea->SetupAttachment(RootComponent);
}

void AWeapon::DrawDebugDamageArea() const
{
	if (!DamageArea) return;

	const FVector CapsuleLocation = DamageArea->GetComponentLocation();
	const float CapsuleRadius = DamageArea->GetScaledCapsuleRadius();
	const float CapsuleHalfHeight = DamageArea->GetScaledCapsuleHalfHeight();
	
	DrawDebugCapsule(GetWorld(), CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, DamageArea->GetComponentQuat(), FColor::Red, false, 10.f, 0, 1);
}

void AWeapon::GetObjectsInDamageArea(const TSubclassOf<AActor> ClassFilter, TArray<AActor*>& Results) const
{
	if(DamageArea)
	{
		DamageArea->GetOverlappingActors(Results, ClassFilter);
	}
}
