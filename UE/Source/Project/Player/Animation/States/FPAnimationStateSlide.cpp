#include "FPAnimationStateSlide.h"

UClass* UFPAnimationStateSlide::Update(float aDT)
{
	Super::Update(aDT);

	const auto defaultTrans = GetDefaultHandTransform();

	// Lerp to camera
	const auto lerpTrans = LerpTransWeight(
		defaultTrans,
		ToCameraSpace(defaultTrans),
		0.5f, 0.5f);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	hands.myLeft = FlipRightToLeft(defaultTrans);
	hands.myPosInterpSpd = 8.0f;
    hands.myRotInterpSpd = 8.0f;
	
	// Apply wobble
	hands.myRight.SetLocation(hands.myRight.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength));
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength, 1000.0f));

	OverrideSwordData(hands, 0.1f, 0.1f, false);
	OverrideVelocityData(hands, 0.5f, aDT);
	SetHands(hands);

	FFPAnimationCameraData camera;
	camera.myHeight = FMath::Cos(GetStateTime() * myCameraBobSpeed) * myCameraBobStrength;
	SetCamera(camera); 
	
	return nullptr;
}
