#include "World/Terrain.h"

#include "PCGComponent.h"
#include "PCGManagedResource.h"
#include "Async/Async.h"
#include "Components/MeshComponent.h"
#include "Game/MyGameLibrary.h"
#include "GeometryFramework/Public/Components/DynamicMeshComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "World/TerrainBuilder.h"
#include "World/WorldMap.h"

ATerrain::ATerrain()
{
	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCG Component"));
	PCGComponent->SetIsPartitioned(false);
	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	GetDynamicMeshComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetDynamicMeshComponent()->EnableComplexAsSimpleCollision();
	GetDynamicMeshComponent()->bUseAsyncCooking = true;
	GetDynamicMeshComponent()->CollisionType = ECollisionTraceFlag::CTF_UseComplexAsSimple;
}

void ATerrain::BeginPlay()
{
	Super::BeginPlay();
	BuildTerrain();
	PCGComponent->SetIsPartitioned(false);
	PCGComponent->Generate();
	PCGComponent->PostGenerateFunctionNames.Add("PostPCGGeneratedFunction");
}

void ATerrain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATerrain, Position);
	DOREPLIFETIME(ATerrain, WorldMap);
	DOREPLIFETIME(ATerrain, OwnedObjects);
}

void ATerrain::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATerrain::Destroyed()
{
	Super::Destroyed();
	
	for (AActor* Object : OwnedObjects)
	{
		if (IsValid(Object))
		{
			Object->Destroy();
		}
	}
}

void ATerrain::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	BuildTerrain();
#endif
}

void ATerrain::RegisterObjectsGeneratedByPCG()
{
	PCGComponent->ForEachManagedResource([&](UPCGManagedResource* Resource)
	{
		if (UPCGManagedActors* ManagedActors = Cast<UPCGManagedActors>(Resource))
		{
			for (TSoftObjectPtr<AActor> Actor : ManagedActors->GeneratedActors)
			{
				OwnedObjects.Add(Actor.Get());
			}
		}
	});
}

void ATerrain::PostPCGGeneratedFunction(FPCGDataCollection Data)
{
	RegisterObjectsGeneratedByPCG();
}

TArray<TerrainType> ATerrain::GetTerrainTypeArray() const
{
	check(WorldMap)
	
	FMapData* MyMap = &WorldMap->WorldMap;
	const FChunkFloor& MyTerrain = MyMap->GetFloorData(Position);
	return MyTerrain.CreateTerrainTypeArray();
}

void ATerrain::BuildTerrainMaterial(const FChunkFloor& FloorData)
{
	const TArray<FColor> ColorBuffer = FloorData.CreateColorArray();
	Texture = UMyGameLibrary::CreateTextureFromColorArray(ColorBuffer, FChunkFloor::GetSize());

	TerrainMaterial = UMaterialInstanceDynamic::Create(SourceMaterial, this);
	TerrainMaterial->SetTextureParameterValue("Terrain Map", Texture);
	TerrainMaterial->SetScalarParameterValue("ChunkRealWidth", AWorldMap::GetChunkWidth());
	TerrainMaterial->SetVectorParameterValue("ChunkPosition", FVector(Position));
	
	GetDynamicMeshComponent()->SetMaterial(0, TerrainMaterial);
	GetDynamicMeshComponent()->SetMaterial(1, BlackMaterial);
}

void ATerrain::SetPosition(const FIntVector InPosition)
{
	Position = InPosition;
}

void ATerrain::BuildTerrain()
{
	if(!BuilderClass || bStartedBuilding || !WorldMap) return;

	bStartedBuilding = true;

	WorldMap->GenerateNeighbourChunks({Position.X, Position.Y});
	FMapData* MyMap = &WorldMap->WorldMap;
	Builder = NewObject<UTerrainBuilder>(this, BuilderClass);
	Builder->Init();
	
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		Builder->Build(WorldMap, Position);
		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			Builder->Finish(GetDynamicMeshComponent());
		 	BuildTerrainMaterial(MyMap->GetFloorData(Position));
			bIsReady = true;
			UpdateNavigation();
		 });
	});
}

void ATerrain::UpdateNavigation()
{
	GetDynamicMeshComponent()->SetCanEverAffectNavigation(false);
	GetDynamicMeshComponent()->SetCanEverAffectNavigation(true);
}

void ATerrain::SetMeshVisibility(UMeshComponent* MeshComp, const bool bVisible)
{
	MeshComp->SetHiddenInGame(!bVisible);
	MeshComp->SetCastHiddenShadow(!bVisible);
}

void ATerrain::SetVisibility(bool bVisibility)
{
	SetMeshVisibility(GetDynamicMeshComponent(), bVisibility);
	SetActorEnableCollision(bVisibility);
	SetObjectsVisibility(bVisibility);
}

void ATerrain::SetObjectsVisibility(bool bVisibility)
{
	for(AActor* Object : OwnedObjects)
	{
		if(Object)
		{
			Object->SetHidden(!bVisibility);
		}
	}
}
