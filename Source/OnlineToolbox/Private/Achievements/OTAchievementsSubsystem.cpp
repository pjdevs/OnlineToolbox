// Fill out your copyright notice in the Description page of Project Settings.
#include "Achievements/OTAchievementsSubsystem.h"

#include "InterchangeResult.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Math/UnitConversion.h"

UOTAchievementsSubsystem::UOTAchievementsSubsystem() :
	AchievementUnlockedDelegate(FOnAchievementUnlockedDelegate::CreateUObject(this, &UOTAchievementsSubsystem::HandleAchievementUnlockedComplete)),
	QueryAchievementsDescCompleteDelegate(FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &UOTAchievementsSubsystem::HandleQueryAchievementsDesc)),
	QueryAchievementsCompleteDelegate(FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &UOTAchievementsSubsystem::HandleQueryAchievements))
{
}

void UOTAchievementsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const auto* Subsystem = IOnlineSubsystem::Get();
	checkf(Subsystem != nullptr, TEXT("Unable to get Online Subsystem"));

	IdentityInterface = Subsystem->GetIdentityInterface();
	checkf(IdentityInterface != nullptr, TEXT("Unable to get Identity Interface"));
	
	AchievementsInterface = Subsystem->GetAchievementsInterface();
	checkf(AchievementsInterface != nullptr, TEXT("Unable to get Achievements Interface"));
	
// #if !UE_BUILD_SHIPPING
// 	auto IdPlayer = IdentityInterface->GetUniquePlayerId(0).Get();
// 	AchievementsInterface->ResetAchievements(*IdPlayer);
// #endif

	AchievementUnlockedDelegateHandle = AchievementsInterface->AddOnAchievementUnlockedDelegate_Handle(AchievementUnlockedDelegate);
}

void UOTAchievementsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	AchievementsInterface->ClearOnAchievementUnlockedDelegate_Handle(AchievementUnlockedDelegateHandle);
}

void UOTAchievementsSubsystem::QueryAllAchievements()
{
	checkf(IdentityInterface != nullptr, TEXT("Unable to get Identity Interface"));
	checkf(AchievementsInterface != nullptr, TEXT("Unable to get Achievements Interface"));

	if (const auto UniquePlayerId = IdentityInterface->GetUniquePlayerId(0))
	{
		AchievementsInterface->QueryAchievementDescriptions(
			*UniquePlayerId,
			QueryAchievementsDescCompleteDelegate);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Try to query achievements without being logged"));
	}
}

void UOTAchievementsSubsystem::HandleQueryAchievements(const FUniqueNetId& PlayerID, const bool bSuccessful) const
{
	if (!bSuccessful)
	{
		OnAchievementsError.Broadcast("Achievements cannot be queried");
		return;
	}

	checkf(IdentityInterface != nullptr, TEXT("Unable to get Identity Interface"));
	checkf(AchievementsInterface != nullptr, TEXT("Unable to get Achievements Interface"));

	// Get the achievement data
	TArray<FOTAchievement> Achievements;
	TArray<FOnlineAchievement> AchievementsData;
	verify(AchievementsInterface->GetCachedAchievements(*IdentityInterface->GetUniquePlayerId(0), AchievementsData) == EOnlineCachedResult::Success);

	// Iterate through the full list of achievements.
	for (auto Data : AchievementsData)
	{
		FOnlineAchievementDesc AchievementDesc;
		verify(AchievementsInterface->GetCachedAchievementDescription(Data.Id, AchievementDesc) == EOnlineCachedResult::Success);

		// Make an achievement usable in Blueprint
		FOTAchievement Achievement;
		Achievement.Id = Data.Id;
		Achievement.Completion = Data.Progress;
		Achievement.Title = AchievementDesc.Title;
		Achievement.LockedDesc = AchievementDesc.LockedDesc;
		Achievement.UnlockedDesc = AchievementDesc.UnlockedDesc;
		Achievement.bIsHidden = AchievementDesc.bIsHidden;
		Achievement.UnlockTime = AchievementDesc.UnlockTime;
		Achievements.Add(Achievement);
	}

	OnAchievementsUpdated.Broadcast(true, Achievements);
}

void UOTAchievementsSubsystem::HandleQueryAchievementsDesc(const FUniqueNetId& PlayerID, const bool bSuccessful)
{
	if (!bSuccessful)
	{
		OnAchievementsError.Broadcast("Achievements descriptions cannot be queried");
		return;
	}

	checkf(IdentityInterface != nullptr, TEXT("Unable to get Identity Interface"));
	checkf(AchievementsInterface != nullptr, TEXT("Unable to get Achievements Interface"));

	AchievementsInterface->QueryAchievements(
		*IdentityInterface->GetUniquePlayerId(0), 
		QueryAchievementsCompleteDelegate);
}

void UOTAchievementsSubsystem::HandleAchievementUnlockedComplete(const FUniqueNetId& NetId, const FString& Response)
{
	UE_LOG(LogTemp, Warning, TEXT("Achievement %s unlocked"), *Response);
	OnAchievementUnlockedComplete.Broadcast(*Response);
}
