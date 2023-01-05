#include "FPAnimationStateHeavyLanding.h"

#include "FPAnimationStateIdle.h"

void UFPAnimationStateHeavyLanding::Enter()
{
	Super::Enter();
	ResetStateTime();
}

UClass* UFPAnimationStateHeavyLanding::Update(const float aDT)
{
	Super::Update(aDT);

	auto hands = GetRealHands();
	hands.myRight = GetDefaultHandTransform();
	hands.myLeft = FlipRightToLeft(hands.myRight);
	hands.myPosInterpSpd = 40.0f;
	hands.myRotInterpSpd = 40.0f;
	OverrideVelocityData(hands, 10.0f);
	OverrideSwordData(hands, 0.3f, 0.7f, false); 
	SetHands(hands, false);

	auto camera = GetRealCamera();
	camera.myHeight = myHeight;
	camera.myTilt = myTilt;
	camera.myHeightInterpSpd = 120.0f;
	SetCamera(camera, false);

	
	if (GetStateTime() > myDuration)
		return UFPAnimationStateIdle::StaticClass();
	
	return nullptr;
}
