
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"

class APCGVolume;
class UPCGComponent;
class ATerrain;
class AGameObject;
class UTerrainBuilder;
class AWorldMap;
class ADataLayerHook;
struct FMapData;

UCLASS()
class MMO_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	AChunk();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AWorldMap> WorldMap;

	UPROPERTY(EditDefaultsOnly)
	float FloorHigh= 300.f;

	UPROPERTY()
	TArray<UTerrainBuilder*> Builders;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> PCGVolumeClass;

	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<ATerrain*> Floors;

	UPROPERTY(VisibleAnywhere)
	FIntPoint GridPosition;

	UPROPERTY()
	FTimerHandle DestroyingTimer;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATerrain> TerrainClass;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void Destroyed() override;

public:
	void Init();
	void SetPosition(const FIntPoint& Position);
	bool IsReady() const;
	void SetFloorsVisibility(int32 From, int32 To, bool bVisibility);
	void SetWorldMap(AWorldMap* Map);
	void TryDestroy();
	void KeepTryingDestroy(AActor* Context);
	ADataLayerHook* GetHook() const;
	AWorldMap* GetWorldMap() const;
};
