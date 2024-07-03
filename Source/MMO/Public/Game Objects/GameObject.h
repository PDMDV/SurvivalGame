
#pragma once

#include "Item.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameObject.generated.h"

class UBoxComponent;

UCLASS()
class MMO_API AGameObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameObject();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> BoundBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameObject> SuccessorClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AItem>> DroppingItems;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere)
	float CurrentHealth = 100.f;

	UFUNCTION(BLueprintPure)
	static FVector GetBoundBoxSize(TSubclassOf<AGameObject> Class);

	UFUNCTION(BLueprintPure)
	static FVector GetBoundBoxLocation(TSubclassOf<AGameObject> Class);

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:	
	void DestroyObject();
};
