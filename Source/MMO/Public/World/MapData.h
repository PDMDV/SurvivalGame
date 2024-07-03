
#pragma once

#include "CoreMinimal.h"
#include "MapData.generated.h"

constexpr int32 GChunkSize = 50;
constexpr int32 GChunkZSize = 8;
constexpr int32 GGridCellLength = 100;

UENUM(BlueprintType)
enum class TerrainType : uint8
{
	Invalid		UMETA(DisplayName = "Invalid"),
	Empty		UMETA(DisplayName = "Empty Space"),
	Flat		UMETA(DisplayName = "Flat Terrain"),
	Occupied	UMETA(DisplayName = "Ocuppied Terrain"),
	Cave		UMETA(DisplayName = "Cave"),
	Max			UMETA(DisplayName = "Max")
};

USTRUCT(BlueprintType)
struct FChunkFloor
{
	GENERATED_BODY()

	FChunkFloor()
	{
		FloorData.Init(TerrainType::Invalid, GChunkSize * GChunkSize);
	}

	UPROPERTY()
	TArray<TerrainType> FloorData;

	TerrainType GetType(const int32 X, const int32 Y) const
	{
		const int32 Index = X + Y * GChunkSize;
		return Index >= 0 && Index < FloorData.Num() ? FloorData[Index] : TerrainType::Invalid;
	}

	void SetType(const int32 X, const int32 Y, const TerrainType Type) { FloorData[X + Y * GChunkSize] = Type; }
	static FIntPoint GetSize() { return {GChunkSize,GChunkSize};};
	TArray<FColor> CreateColorArray() const;
	TArray<TerrainType> CreateTerrainTypeArray() const;
};

USTRUCT(BlueprintType)
struct FChunkData
{
	GENERATED_BODY()

	FChunkData()
	{
		ChunkData.SetNum(GChunkZSize);
	}

	UPROPERTY()
	TArray<FChunkFloor> ChunkData;

	TerrainType GetType(const int32 X, const int32 Y, const int32 Z) const
	{
		return Z >= 0 && Z < GChunkZSize ? ChunkData[Z].GetType(X, Y) : TerrainType::Invalid;
	}

	void SetType(const int32 X, const int32 Y, const int32 Z, const TerrainType Type)
	{
		ChunkData[Z].SetType(X, Y, Type);
	}

	static FIntPoint GetChunkFloorSize() { return {GChunkSize, GChunkSize}; }
	static int32 GetChunkHeight() { return GChunkZSize; }
	static int32 GetChunkRealSize() { return GChunkSize * GGridCellLength; };
};

USTRUCT(BlueprintType)
struct FMapData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TMap<FIntPoint, FChunkData> Chunks;
	
	static void AdjustPosition(int32& LocalPosition, int32& ChunkPosition);
	static int32 GetChunkWidth() { return GChunkSize * GGridCellLength; };
	static int32 GetChunkGridWidth() { return GChunkSize; };
	static int32 GetGridCellLength() { return GGridCellLength; };
	static FIntVector GetInChunkPosition(const FIntVector Position);

	void AddChunk(const FIntPoint& ChunkPosition, const FChunkData& Data) { Chunks.Add(ChunkPosition, Data); };
	bool HasChunk(const FIntPoint& ChunkPosition) const { return Chunks.Contains(ChunkPosition); };
	FChunkFloor& GetFloorData(FIntVector Position);
	FChunkData* GetChunkData(const FIntPoint& ChunkPosition) { return Chunks.Find(ChunkPosition); };
	TerrainType GetType(FIntPoint ChunkPosition, int32 X, int32 Y, int32 Z) const;
	TerrainType GetType(FIntVector Position) const;
};