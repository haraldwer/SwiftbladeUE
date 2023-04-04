#pragma once

#include "CoreMinimal.h"
#include "FPState.generated.h"

UENUM(BlueprintType)
enum class EFPTravelReason : uint8
{
	NONE,
	RESPAWN,
	CHECKPOINT,
	ARENA,
	COUNT
};

struct FFPState
{
	int32 myChapter = 0;
	
	int32 myArenaIndex = 0;
	bool myInArena = false;
	EFPTravelReason myTravelReason = EFPTravelReason::NONE;
	
	int32 myRespawnCount = 0;
	int32 mySeed = 0;
	float myTime = 0;
	
	bool myHasSword = false;
};