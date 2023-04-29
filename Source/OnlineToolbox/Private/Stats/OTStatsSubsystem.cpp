// Fill out your copyright notice in the Description page of Project Settings.


#include "Stats/OTStatsSubsystem.h"

#include "Interfaces/OnlineIdentityInterface.h"

UOTStatsSubsystem::UOTStatsSubsystem()
{
	QueryStatsCompleteDelegate = FOnlineStatsQueryUsersStatsComplete::CreateUObject(this, &UOTStatsSubsystem::HandleGetStatsComplete);
	UpdateStatsCompleteDelegate = FOnlineStatsUpdateStatsComplete::CreateUObject(this, &UOTStatsSubsystem::HandleUpdateStatsComplete);
}

void UOTStatsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const auto* Subsystem = IOnlineSubsystem::Get();
	checkf(Subsystem != nullptr, TEXT("Unable to get the SubSytem"));

	StatsInterface = Subsystem->GetStatsInterface();
	checkf(StatsInterface.IsValid(), TEXT("Unable to get the Stats Interface"));

	IdentityInterface = Subsystem->GetIdentityInterface();
	checkf(IdentityInterface.IsValid(), TEXT("Unable to get the Identity Interface"));
}

void UOTStatsSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UOTStatsSubsystem::UpdateStat(FString StatName, int32 StatValue)
{
	checkf(StatsInterface.IsValid(), TEXT("Unable to get the Stats Interface"));
	checkf(IdentityInterface.IsValid(), TEXT("Unable to get the Identity Interface"));

	if (const auto UniquePlayerId = IdentityInterface->GetUniquePlayerId(0))
	{
		FOnlineStatsUserUpdatedStats UserStats = FOnlineStatsUserUpdatedStats(UniquePlayerId.ToSharedRef());
		UserStats.Stats.Add(StatName, FOnlineStatUpdate(StatValue, FOnlineStatUpdate::EOnlineStatModificationType::Sum));
		TArray<FOnlineStatsUserUpdatedStats> UserStatArray;
		UserStatArray.Add(UserStats);
		StatsInterface->UpdateStats(UniquePlayerId.ToSharedRef(), UserStatArray, UpdateStatsCompleteDelegate);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Try to update stat without being logged in"));
	}
}

void UOTStatsSubsystem::GetStats(const TArray<FString>& StatNames)
{
	checkf(StatsInterface.IsValid(), TEXT("Unable to get the Stats Interface"));
	checkf(IdentityInterface.IsValid(), TEXT("Unable to get the Identity Interface"));

	if (const auto UniquePlayerId = IdentityInterface->GetUniquePlayerId(0))
	{
		TArray<TSharedRef<const FUniqueNetId>> UsersVar;
		UsersVar.Add(UniquePlayerId.ToSharedRef());
		StatsInterface->QueryStats(UniquePlayerId.ToSharedRef(), UsersVar, StatNames, QueryStatsCompleteDelegate);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Try to get stat without being logged in"));
	}
}

void UOTStatsSubsystem::HandleGetStatsComplete(const FOnlineError& Result, const TArray<TSharedRef<const FOnlineStatsUserStats>>& UserStats)
{
	if(Result == FOnlineError::Success())
	{
		for (auto const StatsVar : UserStats)
		{
			for (auto StoredValueRef : StatsVar->Stats)
			{
				const FString Keyname = StoredValueRef.Key;
				int32 Value;
				StoredValueRef.Value.GetValue(Value);

				OnStatGet.Broadcast(Keyname, Value);
			}
		}
	}
	else
		OnStatError.Broadcast(Result.ErrorMessage.ToString());
}

void UOTStatsSubsystem::HandleUpdateStatsComplete(const FOnlineError& Result)
{
	if (Result == FOnlineError::Success())
		OnStatUpdated.Broadcast();
	else
		OnStatError.Broadcast(Result.ErrorMessage.ToString());
}
