#include "EnemyAnimation.h"

void FEnemyAnimationKey::Reset()
{
	myTimer = 0.0f;
}

void FEnemyAnimationKey::Update(const float aDT)
{
	myTimer += aDT;
}

void FEnemyAnimation::Reset()
{
	myNewKey = true;
	for (auto& key : mySequence)
		key.Reset(); 
}

bool FEnemyAnimation::Update(float aDT, TArray<FObjectAnimatorKey>& someKeys, FString& anEventName)
{
	CHECK_RETURN(!mySequence.Num(), false);

	for (int i = 0; i < mySequence.Num(); i++)
	{
		auto& key = mySequence[i];  
		CHECK_CONTINUE(key.HasFinished());
		
		// First time running this key
		if (myNewKey)
		{
			myNewKey = false; 
			anEventName = key.GetEventName();
			someKeys = key.GetObjectKeys();
			return true;
		}
		
		key.Update(aDT);
		
		if (key.HasFinished())
		{
			// Will switch to new key next tick
			myNewKey = true;
		}
		
		return false;
	}

	// Only reaches here if sequence is complete
	if (myLooping)
		Reset();
	
	return false;
}
