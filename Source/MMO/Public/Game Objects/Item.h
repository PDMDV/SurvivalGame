
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "GameplayEffect.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemStateChange : uint8 
{
	Droped UMETA(DisplayName = "Droped"),
	PickedUp UMETA(DisplayName = "PickedUp"),
	ChangedOwner UMETA(DisplayName = "Changed Owner"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateChange, AActor*, Item, EItemStateChange, State);

USTRUCT(BlueprintType)
struct FAttachment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	AActor* Actor;

	UPROPERTY(VisibleAnywhere)
	FGameplayTag Socket;

	UPROPERTY(VisibleAnywhere)
	bool bKeepCollisions;
};

UCLASS()
class MMO_API AItem : public AStaticMeshActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	AItem();

	UPROPERTY(EditDefaultsOnly)
	float PickUpRadius = 150.f;

	UPROPERTY(BlueprintAssignable)
	FStateChange OnStateChanged;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> PickUpEffect;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CarryingEffect;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Attachment)
	FAttachment Attachment;

	UPROPERTY()
	FActiveGameplayEffectHandle PickUpEffectHandle;

	UPROPERTY()
	FActiveGameplayEffectHandle CarryingEffectHandle;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer TagContainer;
	
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& InTagContainer) const override;

	UFUNCTION(BlueprintPure)
	float GetPickUpRadius() const { return PickUpRadius; }
	
	UFUNCTION(BlueprintPure)
	TSubclassOf<UGameplayEffect> GetPickUpEffect() const { return PickUpEffect;};

	UFUNCTION(BlueprintPure)
	FVector GetSocketPosition(FName Name) const;

	UFUNCTION(BlueprintCallable)
	void SetPhysicsAndCollisions(bool bActivePhysics, bool bActiveCollision);

	UFUNCTION(BlueprintCallable)
	bool ApplyPickUpEffect(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void RemovePickUpEffect(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	bool ApplyCarryingEffect(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void RemoveCarryingEffect(AActor* Actor);

	UFUNCTION(BlueprintPure)
	bool IsAttached() const;

	UFUNCTION(BlueprintCallable)
	void AttachTo(AActor* AActor, FGameplayTag SocketTag, bool KeepCollisions);

	UFUNCTION(BlueprintCallable)
	void Detach();

	UFUNCTION(BlueprintCallable)
	void SetAttachment(AActor* Actor, FGameplayTag Socket, bool KeepCollisions);

	UFUNCTION(BlueprintCallable)
	void Drop();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Attachment(FAttachment Previous);
	
};