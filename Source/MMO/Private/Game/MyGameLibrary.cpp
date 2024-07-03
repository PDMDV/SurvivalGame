
#include "Game/MyGameLibrary.h"

#include "Character/CharacterData.h"
#include "Components/BoxComponent.h"
#include "Game Objects/GameObject.h"
#include "Game Objects/Item.h"
#include "Kismet/GameplayStatics.h"
#include "World/MapData.h"
#include "World/WorldMap.h"

bool UMyGameLibrary::IsInRadius(const AActor* First, const AActor* Second, const float Distance)
{
	if(First && Second && Distance > 0.0f)
	{
		return First->GetDistanceTo(Second) < Distance;
	}
	return false;
}

AWorldMap* UMyGameLibrary::GetWorldMap(const UObject* Context)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(Context->GetWorld(), AWorldMap::StaticClass(), OutActors);
	
	if(OutActors.Num() > 0)
	{
		return Cast<AWorldMap>(OutActors[0]);
	}
	return nullptr;
}

TArray<UAnimMontage*> UMyGameLibrary::GetMontagesWithTag(ABaseCharacter* Character, const FGameplayTag Tag)
{
	return Character->CharacterData->GiveMontagesWithTag(Tag);
}

UAnimMontage* UMyGameLibrary::GetRandomMontageWithTag(ABaseCharacter* Character, const FGameplayTag Tag)
{
	TArray<UAnimMontage*> Result = GetMontagesWithTag(Character, Tag);
	if(Result.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, Result.Num() - 1);
		return Result[RandomIndex];
	}
	return nullptr;
}

FGameplayTagContainer UMyGameLibrary::GetOwningChildTags(IGameplayTagAssetInterface* Owner, FGameplayTag Tag)
{
	FGameplayTagContainer Container;
	Owner->GetOwnedGameplayTags(Container);
	return Container.Filter(Tag.GetSingleTagContainer());
}

FGameplayTag UMyGameLibrary::GetRandomOwningChildTag(IGameplayTagAssetInterface* Owner, FGameplayTag Tag)
{
	const FGameplayTagContainer Tags = GetOwningChildTags(Owner, Tag);
	if(Tags.IsEmpty())
	{
		return FGameplayTag();
	}
	return Tags.GetByIndex(FMath::RandRange(0,Tags.Num() - 1));
}

UTexture2D* UMyGameLibrary::CreateTextureFromColorArray(TArray<FColor> Data, const FIntPoint& Size)
{
	UTexture2D* Texture = UTexture2D::CreateTransient(Size.X, Size.Y, PF_B8G8R8A8);
	if (!Texture)
	{
		return nullptr;
	}

#if WITH_EDITORONLY_DATA
	Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	Texture->NeverStream = true;
	Texture->SRGB = 0;
	Texture->Filter = TextureFilter::TF_Nearest;
	Texture->AddressX = TextureAddress::TA_Mirror;
	Texture->AddressY = TextureAddress::TA_Mirror;

	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);

	FMemory::Memcpy(TextureData, Data.GetData(), Size.X * Size.Y * 4);
	Mip.BulkData.Unlock();
	Texture->UpdateResource();

	return Texture;
}

uint8 UMyGameLibrary::GetTerrainArrayIndex(const TerrainType Type) // TODO Create TerrainType Class
{
	switch (Type)
	{
	case TerrainType::Flat:
		return 0;
	case TerrainType::Cave:
		return 1;

		// ... //

		default:
			return 255;
	}
}