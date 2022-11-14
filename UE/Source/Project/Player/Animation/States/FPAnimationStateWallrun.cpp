#include "FPAnimationStateWallrun.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Movement/FPMovement.h"
#include "Project/Player/Movement/States/FPMovementStateWall.h"

UClass* UFPAnimationStateWallrun::Update(float aDT)
{
	Super::Update(aDT);

	FFPAnimationHandPositions hands;
	FFPAnimationCameraData camera;
	
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

	// Wall normal specific 
	if (const auto wallrun = GetMovement().GetState<UFPMovementStateWall>())
	{
		const auto wallNormal = wallrun->GetWallNormal();
		const auto& character = GetCharacter();
		const auto right = character.GetActorRightVector();
		const auto dot = -wallNormal.Dot(right);

		// Extend arm
		const float weight = FMath::Abs(dot);		
		const auto extendedTrans = dot > 0.0f ?
			myExtendedHandTrans :
			FlipRightToLeft(myExtendedHandTrans);
		auto& hand = dot > 0.0f ?
			hands.myRight : hands.myLeft;
		hand = LerpTransWeight(
			hand,
			extendedTrans,
			weight,
			weight);

		// Tilt head
		camera.myTilt = dot * FMath::DegreesToRadians(myHeadWallTilt);
	}
	
	// Find close collisions
	const FFPAnimationHandCollision rightResult = GetHandCollision(hands.myRight, 15.0f);
	if (rightResult.myHit)
		hands.myRight = rightResult.myTransform;
	const FFPAnimationHandCollision leftResult = GetHandCollision(hands.myLeft, 1.0f);
	if (leftResult.myHit)
		hands.myLeft.SetLocation(leftResult.myTransform.GetLocation() + leftResult.myTransform.Rotator().Vector() * 20.0f);

	OverrideSwordData(hands, 0.8f, 0.9f, false);
	SetHands(hands);

	// Camera
	camera.myHeight = FMath::Abs(cos) * myHeadBobStrength;
	camera.myTilt += cos * FMath::DegreesToRadians(myHeadTiltAmount);
	SetCamera(camera);
	
	return nullptr;
}
