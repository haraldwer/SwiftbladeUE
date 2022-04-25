#include "FPAnimationStateInAir.h"

UClass* UFPAnimationStateInAir::Update(float aDT)
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

	// Find close collisions
	const FFPAnimationHandCollision rightResult = GetHandCollision(hands.myRight, 50.0f);
	if (rightResult.myHit)
		hands.myRight = rightResult.myTransform;
	const FFPAnimationHandCollision leftResult = GetHandCollision(hands.myLeft, 50.0f);
	if (leftResult.myHit)
	{
		hands.myLeft = leftResult.myTransform;
		// Flip rotation
		const FRotator normal = hands.myLeft.Rotator();
		const FRotator flipped = FlipRightToLeft(hands.myLeft).Rotator();
		hands.myLeft.SetRotation(FRotator(flipped.Pitch, normal.Yaw, flipped.Roll).Quaternion());
	}
	
	// Apply wobble
	if (!rightResult.myHit)
		hands.myRight.SetLocation(hands.myRight.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength));
	if (!leftResult.myHit)
		hands.myLeft.SetLocation(hands.myLeft.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength, 1000.0f));

	OverrideSwordData(hands, 0.3f, 0.5f, false);
	OverrideVelocityData(hands, 0.5f, aDT);
	SetHands(hands);

	FFPAnimationCameraData camera;
	camera.myHeight = FMath::Cos(GetStateTime() * myCameraBobSpeed) * myCameraBobStrength;
	SetCamera(camera); 
	
	return nullptr;
}
