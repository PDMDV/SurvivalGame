
#pragma once

#include "CoreMinimal.h"
#include "TerrainBuilder.h"
#include "WorldMap.h"
#include "GameFramework/Actor.h"
#include "Terrain.generated.h"

class UDynamicMeshComponent;
class UTerrainBuilder;
class UProceduralMeshComponent;
class UMeshComponent;
class UCanvasRenderTarget2D;
class AGameObject;
struct FChunkFloor;

UCLASS()
class MMO_API ATerrain : public ADynamicMeshActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	bool bStartedBuilding = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsReady = false;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterial> BlackMaterial;

	UPROPERTY(Transient, Replicated)
	TArray<AActor*> OwnedObjects;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPCGComponent> PCGComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> SourceMaterial;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTerrainBuilder> BuilderClass = UTerrainBuilder::StaticClass();

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> TerrainMaterial;

	UPROPERTY()
	UTerrainBuilder* Builder;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> Texture;

	UPROPERTY(VisibleAnywhere, Replicated)
	FIntVector Position;

	UPROPERTY(VisibleAnywhere, Replicated)
	AWorldMap* WorldMap;

	ATerrain();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void RegisterObjectsGeneratedByPCG();

public:
	UFUNCTION(CallInEditor)
	void PostPCGGeneratedFunction(FPCGDataCollection Data);
	
	UFUNCTION(BlueprintCallable)
	TArray<TerrainType> GetTerrainTypeArray() const;

	UFUNCTION(BlueprintPure)
	bool IsReady() const {return bIsReady; };

	static void SetMeshVisibility(UMeshComponent* MeshComp, bool bVisible);

	void BuildTerrainMaterial(const FChunkFloor& FloorData);
	void SetPosition(FIntVector InPosition);
	void AddObject(AActor* Object) {OwnedObjects.Add(Object);};
	void RemoveObject(AActor* Object) {OwnedObjects.Remove(Object);};
	void BuildTerrain();
	void UpdateNavigation();
	void SetVisibility(bool bVisibility);
	void SetObjectsVisibility(bool bVisibility);
};
