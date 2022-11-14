#include "FPAnimationStateWallclimb.h"

#include "GameFramework/CharacterMovementComponent.h"

UClass* UFPAnimationStateWallclimb::Update(float aDT)
{
	Super::Update(aDT);

	const float cos = FMath::Cos(GetStateTime() * mySpeed);
	const auto defaultTrans = GetDefaultHandTransform();
	
	// Lerp to camera
	const auto lerpTrans = LerpTransWeight(
		defaultTrans,
		ToCameraSpace(defaultTrans),
		0.5f, 0.5f);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	hands.myLeft = FlipRightToLeft(hands.myRight);

	// Swing arms
	const FVector rightOffset = mySwingVector * cos * mySwingAmount;
	const FVector leftOffset = mySwingVector * -cos * mySwingAmount;
	hands.myRight.SetLocation(hands.myRight.GetLocation() + rightOffset);
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() + leftOffset);
	hands.myRight.SetRotation(hands.myRight.GetRotation() *
		FRotator(cos * mySwingHandRotateAmount, 0.0f, 0.0f).Quaternion());
	hands.myLeft.SetRotation(hands.myLeft.GetRotation() *
		FRotator(-cos * mySwingHandRotateAmount, 0.0f, 0.0f).Quaternion());
	
	// Find close collisions
	const FFPAnimationHandCollision rightResult = GetHandCollision(hands.myRight);
	if (rightResult.myHit)
		hands.myRight = rightResult.myTransform;
	const FFPAnimationHandCollision leftResult = GetHandCollision(hands.myLeft);
	if (leftResult.myHit)
	{
		hands.myLeft = leftResult.myTransform;
		// Flip rotation
		const FRotator normal = hands.myLeft.Rotator();
		const FRotator flipped = FlipRightToLeft(hands.myLeft).Rotator();
		hands.myLeft.SetRotation(FRotator(flipped.Pitch, normal.Yaw, flipped.Roll).Quaternion());
	}

	OverrideSwordData(hands, 0.3f, 0.4f, false);
	SetHands(hands);

	// Camera
	FFPAnimationCameraData camera;
	camera.myTilt = cos * FMath::DegreesToRadians(myHeadTiltAmount);
	SetCamera(camera);
	
	return nullptr;
}
