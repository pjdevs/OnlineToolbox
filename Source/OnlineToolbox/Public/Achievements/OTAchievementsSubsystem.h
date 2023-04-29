// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OTAchievement.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "OTAchievementsSubsystem.generated.h"

// Fill out your copyright notice in the Description page of Project Settings.

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOTAchievementsUpdatedDelegate, bool, bSuccessful, const TArray<FOTAchievement>&, Achievements);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOTAchievementUnlockedDelegate, FString, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOTAchievementsErrorDelegate, FString, Error);

/**
 * 
 */
UCLASS()
class ONLINETOOLBOX_API UOTAchievementsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	explicit UOTAchievementsSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UPROPERTY(BlueprintAssignable, Category="Achievements")
	FOTAchievementsUpdatedDelegate OnAchievementsUpdated;
	
	UPROPERTY(BlueprintAssignable, Category="Achievements")
	FOTAchievementUnlockedDelegate OnAchievementUnlockedComplete;

	UPROPERTY(BlueprintAssignable, Category="Achievements")
	FOTAchievementsErrorDelegate OnAchievementsError;

	UFUNCTION(BlueprintCallable, Category="Achievements")
	void QueryAllAchievements();

private:
	IOnlineAchievementsPtr AchievementsInterface;
	IOnlineIdentityPtr IdentityInterface;

	FDelegateHandle AchievementUnlockedDelegateHandle;
	FOnAchievementUnlockedDelegate AchievementUnlockedDelegate;
	FOnQueryAchievementsCompleteDelegate QueryAchievementsDescCompleteDelegate;
	FOnQueryAchievementsCompleteDelegate QueryAchievementsCompleteDelegate;

	void HandleQueryAchievements(const FUniqueNetId& PlayerID, const bool bSuccessful) const;
	void HandleQueryAchievementsDesc(const FUniqueNetId& PlayerID, const bool bSuccessful);
	void HandleAchievementUnlockedComplete(const FUniqueNetId& NetId, const FString& Response);
};
