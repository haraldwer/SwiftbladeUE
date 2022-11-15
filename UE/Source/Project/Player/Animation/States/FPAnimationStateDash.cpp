#include "FPAnimationStateDash.h"

void UFPAnimationStateDash::Enter()
{
	Super::Enter();
	ResetStateTime();
}

UClass* UFPAnimationStateDash::Update(float aDT)
{
	Super::Update(aDT);

	const auto defaultTrans = GetDefaultHandTransform();

	// Lerp to camera
	const auto lerpTrans = LerpTransWeight(
		defaultTrans,
		ToCameraSpace(defaultTrans),
		0.8f, 0.8f);
	
	FFPAnimationHandPositions hands;
	hands.myPosInterpSpd = 30.0f;
	hands.myRotInterpSpd = 30.0f; 
	hands.myRight = lerpTrans;
	hands.myLeft = FlipRightToLeft(hands.myRight);
	
	OverrideSwordData(hands, 0.2f, 0.4f, false);
	OverrideVelocityData(hands, -0.2f);
	SetHands(hands);

	FFPAnimationCameraData camera;
	SetCamera(camera); 
	
	return nullptr;
}
