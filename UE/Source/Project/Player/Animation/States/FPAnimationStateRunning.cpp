#include "FPAnimationStateRunning.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Utility/Tools/CustomCamera.h"

UClass* UFPAnimationStateRunning::Update(float aDT)
{
	Super::Update(aDT);

	const auto defaultTrans = GetDefaultHandTransform();

	// Calculate speed
	const float speed = GetCharacterMovement().Velocity.Length() / myStepSize;
	myDist += speed * aDT;
	const float cos = FMath::Cos(myDist); 
	
	// Lerp to camera
	const auto lerpTrans = LerpTransWeight(
		defaultTrans,
		ToCameraSpace(defaultTrans),
		0.5f, 0.5f);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	hands.myLeft = FlipRightToLeft(hands.myRight);
	hands.myRightHandState = GetSwordHandState();
	hands.myLeftHandState = EHandState::OPEN;

	// Swing arms
	const FVector swingDirection = (GetCamera().GetForwardVector() + GetCamera().GetUpVector()).GetSafeNormal();
	const FVector rightOffset = swingDirection * cos * mySwingAmount;
	const FVector leftOffset = swingDirection * -cos * mySwingAmount;
	hands.myRight.SetLocation(hands.myRight.GetLocation() + rightOffset);
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() + leftOffset);

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
	
	SetHands(hands);

	// Camera
	FFPAnimationCameraData camera;
	camera.myHeight = FMath::Abs(cos) * myHeadBobStrength;
	camera.myTilt = cos * myHeadTiltAmount;
	SetCamera(camera);
	
	return nullptr;
}
