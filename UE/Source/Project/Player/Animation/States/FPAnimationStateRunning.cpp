#include "FPAnimationStateRunning.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/FPCharacter.h"

UClass* UFPAnimationStateRunning::Update(float aDT)
{
	Super::Update(aDT);

	const auto defaultTrans = GetDefaultHandTransform();

	// Calculate speed
	const float speed = GetCharacterMovement().Velocity.Length() / myStepSize;

	const float previousCos = FMath::Cos(myDist); 
	myDist += speed * aDT;
	const float cos = FMath::Cos(myDist);

	// Footsteps
	float cosDiff = cos - previousCos;
	bool increasing = cosDiff < 0;
	if (increasing != myCosIncreasing)
	{
		myCosIncreasing = increasing; 
		// Shift step
		FTransform footstepTrans = increasing ? myLeftFoot : myRightFoot;
		auto& character = GetCharacter();
		character.CreateEffect(myFootstepBP, footstepTrans * character.GetTransform());
	}
	
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

	OverrideSwordData(hands, 0.8f, 0.9f, false);
	OverrideVelocityData(hands, 0.7f, aDT);
	SetHands(hands);

	// Camera
	FFPAnimationCameraData camera;
	camera.myHeight = FMath::Abs(cos) * myHeadBobStrength;
	camera.myTilt = cos * FMath::DegreesToRadians(myHeadTiltAmount);
	camera.myTilt -= GetVelocityCameraTilt() * FMath::DegreesToRadians(myVelocityCameraTilt); 
	SetCamera(camera);
	
	return nullptr;
}
