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
	FString myUserID = "";
	UPROPERTY(BlueprintReadOnly)
	int32 myRank = -1;
	UPROPERTY(BlueprintReadOnly)
	int32 myScore = -1;
	UPROPERTY(BlueprintReadOnly)
	int32 mySeed = -1;
};

USTRUCT(BlueprintType)
struct FLeaderboardData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ELeaderboardType myType = ELeaderboardType::PERSONAL;
	UPROPERTY(BlueprintReadOnly)
	ELeaderboardSeed mySeedType = ELeaderboardSeed::ANY;
	UPROPERTY(BlueprintReadOnly)
	int32 mySeed = -1;
	UPROPERTY(BlueprintReadOnly)
	TArray<FLeaderboardEntry> myEntries; 
};

// - Submissions - // 

USTRUCT()
struct FLeaderboardSubmission
{
	GENERATED_BODY()

	UPROPERTY()
	int32 myScore = 0.0f;
	UPROPERTY()
	int32 mySeed = -1;
};

// - Requests - //

USTRUCT(BlueprintType)
struct FLeaderboardRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ELeaderboardType myType = ELeaderboardType::PERSONAL;
	UPROPERTY(BlueprintReadWrite)
	ELeaderboardSeed mySeedType = ELeaderboardSeed::ANY;
	UPROPERTY(BlueprintReadWrite)
	int32 mySeed = -1;
	
	UPROPERTY()
	int32 myEntries = 10;

	bool operator==(const FLeaderboardRequest& anOther) const
	{
		return myType == anOther.myType &&
			mySeedType == anOther.mySeedType &&
			mySeed == anOther.mySeed &&
			myEntries == anOther.myEntries;
	}
};