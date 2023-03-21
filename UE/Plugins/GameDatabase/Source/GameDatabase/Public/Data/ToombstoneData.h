#pragma once

#include "CoreMinimal.h"
#include "ToombstoneData.generated.h"

USTRUCT()
struct FToombstoneEntry
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 mySeed;
	
	UPROPERTY()
	FVector myLocation;
	
	UPROPERTY()
	int32 myArenaIndex;

	UPROPERTY()
	bool myInArena;
};

USTRUCT()
struct FBlobToombstone
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FToombstoneEntry> myToombstones; 
};