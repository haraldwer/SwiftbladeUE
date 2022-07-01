#include "FPAnimationStateDeath.h"

UClass* UFPAnimationStateDeath::Update(float aDT)
{
	Super::Update(aDT);

	const auto defaultTrans = GetDefaultHandTransform();

	// Lerp to camera
	const auto lerpTrans = LerpTransWeight(
		defaultTrans,
		ToCameraSpace(defaultTrans),
		1.0f, 1.0f);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	hands.myLeft = GetRealHands().myLeft;
	hands.myPosInterpSpd = 20.0f;
    hands.myRotInterpSpd = 20.0f;

	OverrideSwordData(hands, 0.0f, 0.0f, false);
	SetHands(hands, false);

	return nullptr;
}
