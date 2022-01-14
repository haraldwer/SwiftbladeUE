#pragma once

#include "CoreMinimal.h"
#include "Project/ObjectAnimatorKey.h"
#include "EnemyAnimation.generated.h"

// Note:
// Using structs because of data structure
// and poor object management in unreal

USTRUCT()
struct FEnemyAnimationKey
{
	GENERATED_BODY()
	
	void Reset();
	void Update(float aDT);
	bool HasFinished() const { return myTimer >= myDuration; }
	const TArray<FObjectAnimatorKey>& GetObjectKeys() const { return myObjectKeys; }
	const FString& GetEventName() const { return myEventName; }

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FObjectAnimatorKey> myObjectKeys;
	
	UPROPERTY(EditDefaultsOnly)
	float myDuration = -1.0f;

	UPROPERTY(EditDefaultsOnly)
	FString myEventName;

private:
	float myTimer = 0.0f;
};

USTRUCT()
struct FEnemyAnimation
{
	GENERATED_BODY()

	void Reset();
	bool Update(float aDT, TArray<FObjectAnimatorKey>& someKeys, FString& anEventName);

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FEnemyAnimationKey> mySequence;
	
	UPROPERTY(EditDefaultsOnly)
	bool myLooping = false;

private:
	bool myNewKey = true;
};
