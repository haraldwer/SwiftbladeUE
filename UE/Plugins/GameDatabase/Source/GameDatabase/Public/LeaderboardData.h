#pragma once

#include "CoreMinimal.h"
#include "LeaderboardData.generated.h"

USTRUCT(BlueprintType)
struct FLeaderboardEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString myName = "";
	
	UPROPERTY(BlueprintReadOnly)
	float myTime = 0.0f;
	
	UPROPERTY(BlueprintReadOnly)
	int32 mySeed = -1; 
};

USTRUCT(BlueprintType)
struct FLeaderboardData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FLeaderboardEntry> myEntries; 
};