#pragma once

#include "CoreMinimal.h"
#include "LeaderboardData.generated.h"

// - Common types - //

UENUM(BlueprintType)
enum class ELeaderboardType : uint8
{
	PERSONAL,
	FRIENDS,
	GLOBAL
};

UENUM(BlueprintType)
enum class ELeaderboardSeed : uint8
{
	ANY,
	LAST_PLAYED,
	SPECIFIC
};

// - Recieved data - // 

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
	ELeaderboardType myType;

	UPROPERTY(BlueprintReadOnly)
	ELeaderboardSeed mySeed;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FLeaderboardEntry> myEntries; 
};

// - Submissions - // 

USTRUCT()
struct FLeaderboardSubmission
{
	GENERATED_BODY()

	UPROPERTY()
	float myTime = 0.0f;

	UPROPERTY()
	int32 mySeed = -1;
	
};

// - Requests - //

USTRUCT()
struct FLeaderboardRequest
{
	GENERATED_BODY()

	UPROPERTY()
	ELeaderboardSeed mySeed;
	UPROPERTY()
	ELeaderboardType myType;
	
};