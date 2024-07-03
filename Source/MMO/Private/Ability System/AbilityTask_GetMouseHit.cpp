
#include "Ability System/AbilityTask_GetMouseHit.h"
#include "AbilitySystemComponent.h"

UAbilityTask_GetMouseHit* UAbilityTask_GetMouseHit::CreateGetMouseHit(UGameplayAbility* OwningAbility, FName TaskInstanceName, bool OnlyTriggerOnce, bool SendTargetDataToServer)
{
	UAbilityTask_GetMouseHit* MyObject = NewAbilityTask<UAbilityTask_GetMouseHit>(OwningAbility, TaskInstanceName);

	MyObject->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObject->bTickingTask = !OnlyTriggerOnce;
	MyObject->SendTargetDataToServer = SendTargetDataToServer;

	return MyObject;
}

FGameplayAbilityTargetData_SingleTargetHit*  UAbilityTask_GetMouseHit::GetHitTargetData() const
{
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	
	if(const APlayerController* PlayerController = Ability->GetActorInfo().PlayerController.Get())
	{
		PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, TargetData->HitResult);
	}
	return TargetData;
}

void UAbilityTask_GetMouseHit::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag Tag)
{
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		MouseDataReceived.Broadcast(DataHandle);
		if(OnlyTriggerOnce)
		{
			EndTask();
		}
	}
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());
}

void UAbilityTask_GetMouseHit::ServerReceiveTargetData()
{
	AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey())
			.AddUObject(this, &UAbilityTask_GetMouseHit::OnTargetDataReceived);
	const bool bIsCalled = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(GetAbilitySpecHandle(),GetActivationPredictionKey());
	if(bIsCalled)
	{
		SetWaitingOnRemotePlayerData();
	}
}

void UAbilityTask_GetMouseHit::TickTask(float DeltaTime)
{
	if(IsLocallyControlled())
	{
		SendTargetData();
	}
	else if (!IsWaitingOnRemotePlayerdata())
	{
		ServerReceiveTargetData();
	}
}

void UAbilityTask_GetMouseHit::SendTargetData()
{
	FScopedPredictionWindow PredictionWindow(AbilitySystemComponent.Get());

	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(GetHitTargetData());
	if(SendTargetDataToServer)
	{
		AbilitySystemComponent->ServerSetReplicatedTargetData
		(
			GetAbilitySpecHandle(),
			GetActivationPredictionKey(),
			DataHandle,
			FGameplayTag(),
			AbilitySystemComponent->ScopedPredictionKey
		);
	}


	if(ShouldBroadcastAbilityTaskDelegates())
	{
		MouseDataReceived.Broadcast(DataHandle);
		if(OnlyTriggerOnce)
		{
			EndTask();
		}
	}
}

void UAbilityTask_GetMouseHit::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_GetMouseHit::BeginDestroy()
{
	Super::BeginDestroy();
}

void UAbilityTask_GetMouseHit::Activate()
{
	if(IsLocallyControlled())
	{
		SendTargetData();
	}
	else
	{
		ServerReceiveTargetData();
	}
}
