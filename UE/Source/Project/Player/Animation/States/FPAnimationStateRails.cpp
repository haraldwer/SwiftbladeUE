#include "FPAnimationStateRails.h"

#include "Project/Player/FPCharacter.h"
#include "Project/Player/Movement/FPMovement.h"
#include "Project/Player/Movement/States/FPMovementStateRails.h"

UClass* UFPAnimationStateRails::Update(float aDT)
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
	hands.myLeft = FlipRightToLeft(lerpTrans);
	hands.myPosInterpSpd = 15.0f;
    hands.myRotInterpSpd = 15.0f;

	// Apply wobble
	hands.myRight.SetLocation(hands.myRight.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength));
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength));

	OverrideSwordData(hands, 0.3f, 0.5f, false);
	OverrideVelocityData(hands, 0.5f, aDT);

	if (const auto railState = GetMovement().GetState<UFPMovementStateRails>())
	{
		const auto splineTrans = railState->GetSplineTransform();
		
		const FVector charForward = (GetCharacter().GetActorForwardVector() * FVector(1.0f, 1.0f, 0.0f)).GetSafeNormal();
		const FVector splineForward = (splineTrans.GetRotation().Vector() * FVector(1.0f, 1.0f, 0.0f)).GetSafeNormal();
		bool isFlipped = FVector::DotProduct(charForward, splineForward) < 0.0f;
		
		auto handTrans = splineTrans * GetCharacter().GetTransform().Inverse();
		hands.myLeft.SetRotation(handTrans.GetRotation() * FRotator(90.0f, 0.0f, 180.0f * isFlipped).Quaternion());
		hands.myLeft.SetLocation(handTrans.GetLocation() + myHandOffset);
	}
	
	SetHands(hands);

	FFPAnimationCameraData camera;
	camera.myHeight = FMath::Cos(GetStateTime() * myCameraBobSpeed) * myCameraBobStrength;
	camera.myTilt = -GetVelocityCameraTilt() * FMath::DegreesToRadians(myVelocityCameraTilt);
	SetCamera(camera); 
	
	return nullptr;
}
