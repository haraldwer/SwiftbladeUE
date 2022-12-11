#pragma once

#include "CoreMinimal.h"
#include "LeaderboardData.h"
#include "BlobData.generated.h"

// Each data type is synced automatically when changed

USTRUCT()
struct FBlobSeed
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 mySeed = 0;
};

USTRUCT()
struct FBlobLB
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FLeaderboardEntry> myEntries;
};

USTRUCT()
struct FBlobData
{
	GENERATED_BODY()

	UPROPERTY()
	bool myInitialized = false; 
	
	UPROPERTY()
	FBlobSeed mySeedData;

	UPROPERTY()
	FBlobLB myLBData;
};
