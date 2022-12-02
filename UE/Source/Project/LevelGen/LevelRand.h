#pragma once

#include "CoreMinimal.h"
#include "LevelRand.generated.h"

UCLASS()
class ULevelRand : public UObject 
{
	GENERATED_BODY()
	
public:

	static void Init(const int32 aSeed)
	{
		myStream.Initialize(aSeed);
		mySeed = aSeed;
		LOG("Init Seed: " + FString::FromInt(aSeed));
	}

	static void Reset(int32 anOffset = 0)
	{
		myStream.Initialize(mySeed + anOffset);
	}

	UFUNCTION(BlueprintCallable)
	static int32 RandRange(const int32 aMin, const int32 aMax)
	{
		return myStream.RandRange(aMin, aMax);
	}

	UFUNCTION(BlueprintCallable)
	static float FRandRange(const float aMin, const float aMax) 
	{
		return myStream.FRandRange(aMin, aMax);
	}
	
	UFUNCTION(BlueprintCallable)
	static FVector RandVec()
	{
		return myStream.VRand();
	}
	
private:

	inline static int32 mySeed = 0;
	inline static FRandomStream myStream;
	
};
