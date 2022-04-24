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
		0.5f, 0.5f);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	hands.myLeft = FlipRightToLeft(hands.myRight);

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
	
	OverrideSwordData(hands, 0.8f, 1.0f);
	SetHands(hands);

	FFPAnimationCameraData camera;
	SetCamera(camera); 
	
	return nullptr;
}
