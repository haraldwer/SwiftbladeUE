#include "FPAnimationStateSlide.h"

void UFPAnimationStateSlide::Enter()
{
	return;
	const FVector offset(0, 0, 50);
	auto hands = GetRealHands();
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() + offset);
	hands.myRight.SetLocation(hands.myRight.GetLocation() + offset);
	SetHands(hands, true);
}

void UFPAnimationStateSlide::Exit()
{
	// Move hands?
	const FVector offset(0, 0, -50);
	auto hands = GetRealHands();
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() + offset);
	hands.myRight.SetLocation(hands.myRight.GetLocation() + offset);
	SetHands(hands, true);
}

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
	hands.myLeft = FlipRightToLeft(hands.myRight);
	hands.myPosInterpSpd = 5.0f;
    hands.myRotInterpSpd = 5.0f;
	
	// Apply wobble
	hands.myRight.SetLocation(hands.myRight.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength));
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength, 1000.0f));

	OverrideSwordData(hands, 0.8f, 0.5f, false);
	OverrideVelocityData(hands, 0.5f, aDT);
	SetHands(hands);

	FFPAnimationCameraData camera;
	camera.myHeight = FMath::Cos(GetStateTime() * myCameraBobSpeed) * myCameraBobStrength;
	SetCamera(camera); 
	
	return nullptr;
}
