// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "OTStatsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOTStatUpdatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOTStatGetDelegate, FString, Keyb, int, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOTStatErrorDelegate, FString, Error);

UCLASS()
class ONLINETOOLBOX_API UOTStatsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	explicit UOTStatsSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="Stats")
	void UpdateStat(FString StatName, int32 StatValue);

	UFUNCTION(BlueprintCallable, Category="Stats")
	void GetStats(const TArray<FString>& StatNames);

	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOTStatUpdatedDelegate OnStatUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOTStatGetDelegate OnStatGet;
	
	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOTStatErrorDelegate OnStatError;

private:
	FOnlineStatsQueryUsersStatsComplete QueryStatsCompleteDelegate;
	FOnlineStatsUpdateStatsComplete UpdateStatsCompleteDelegate;

	IOnlineStatsPtr    StatsInterface;
	IOnlineIdentityPtr IdentityInterface;
	void HandleGetStatsComplete(const FOnlineError& Result, const TArray<TSharedRef<const FOnlineStatsUserStats>>& UserStats);
	void HandleUpdateStatsComplete(const FOnlineError& Result);
};
