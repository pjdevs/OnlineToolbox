// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTAchievement.h"
#include "UObject/Object.h"
#include "OTAchievementObject.generated.h"

/**
 * UObject wrapper for an achievement.
 */
UCLASS(BlueprintType)
class ONLINETOOLBOX_API UOTAchievementObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Achievements")
	FOTAchievement Achievement;
};
