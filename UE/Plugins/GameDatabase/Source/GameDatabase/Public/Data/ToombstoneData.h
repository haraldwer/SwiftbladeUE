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
	FVector_NetQuantize myLocation;
};

USTRUCT()
struct FBlobToombstone
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FToombstoneEntry> myToombstones; 
};