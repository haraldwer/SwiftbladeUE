#include "FPAnimationStateHug.h"

void UFPAnimationStateHug::Enter()
{
	Super::Enter();
	myOffset = FVector::ZeroVector; 
}

UClass* UFPAnimationStateHug::Update(float aDT)
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

	hands.myRight.SetLocation(hands.myRight.GetLocation() + myOffset * FVector(0.0f, 1.0f, 1.0f));
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() + myOffset * FVector(0.0f, 1.0f, 1.0f));
	
	hands.myRightHandState = EHandState::CLOSED;
	hands.myLeftHandState = EHandState::CLOSED;
	hands.myPosInterpSpd = 10.0f;
    hands.myRotInterpSpd = 10.0f;

	OverrideSwordData(hands, 0.0f, 0.0f, false);
	SetHands(hands);

	FFPAnimationCameraData camera;
	camera.myTilt = myOffset.X;
	SetCamera(camera); 
	
	return nullptr;
}

void UFPAnimationStateHug::Wiggle(float aScale)
{
	myOffset = FVector(
		FMath::RandRange(-myWiggleOffset.X, myWiggleOffset.X),
		FMath::RandRange(-myWiggleOffset.Y, myWiggleOffset.Y),
		FMath::RandRange(-myWiggleOffset.Z, myWiggleOffset.Z)
	) * aScale;
}
