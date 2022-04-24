#include "FPAnimationStateStrike.h"

#include "Curves/CurveFloat.h"

UClass* UFPAnimationStateStrike::Update(float aDT)
{
	Super::Update(aDT);

	const float timeDiff = GetTime() - myStrikeTimestamp; 
	const float timePart = FMath::Clamp(timeDiff / myStrikeDuration, 0.0f, 1.0f);
	
	auto trans = GetDefaultHandTransform();

	// Get entry trans
	if (myEntries.IsValidIndex(myAnimIndex))
	{
		const auto& entry = myEntries[myAnimIndex];

		
		const float weight =
			myCurve ? myCurve->GetFloatValue(timePart) : timePart;
		trans = LerpTransWeight(
			entry.myStart,
			entry.myEnd,
			weight,
			weight);
	}
	
	// Lerp to camera
	const auto lerpTrans = LerpTransWeight(
		trans,
		ToCameraSpace(trans),
		0.8f, 0.8f);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans; 
	hands.myDualWeild = true;
	
	OverrideSwordData(hands, 0.0f, 0.0f);
	SetHands(hands, true);

	FFPAnimationCameraData camera;
	SetCamera(camera); 
	
	return nullptr;
}

void UFPAnimationStateStrike::Enter()
{
	Super::Enter();
	myStrikeTimestamp = GetTime();
	myAnimIndex = FMath::RandRange(0, myEntries.Num());
}
