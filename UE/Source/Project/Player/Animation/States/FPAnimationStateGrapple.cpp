#include "FPAnimationStateGrapple.h"

#include "Project/Player/Movement/FPMovement.h"
#include "Project/Player/Movement/States/FPMovementStateGrapple.h"

void UFPAnimationStateGrapple::Enter()
{
	Super::Enter();
	ResetStateTime();
}

UClass* UFPAnimationStateGrapple::Update(float aDT)
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

	// Apply wobble
	hands.myRight.SetLocation(hands.myRight.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength));
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength, 1000.0f));
	
	// Reach left towards extended 
	if (auto grappleState = GetMovement().GetState<UFPMovementStateGrapple>())
	{
		if (grappleState->GetIsInFreeze())
		{
			// Extend
			hands.myPosInterpSpd = 30.0f;
			hands.myRotInterpSpd = 30.0f;

			const FTransform actorTrans = GetActorTransform();
			const FVector direction = (grappleState->GetTargetLocation() - actorTrans.GetLocation()).GetSafeNormal();
			const FVector localDirection = actorTrans.InverseTransformVectorNoScale(direction);
			const FVector adjusted = (localDirection + myExtendCompensation).GetSafeNormal();
			hands.myLeft.SetLocation(hands.myLeft.GetLocation() + adjusted * myExtendedDist);
			hands.myLeft.SetRotation(myExtendedRotation);
		}
		else
		{
			// Pull
			hands.myPosInterpSpd = 5.0f;
			hands.myRotInterpSpd = 5.0f;
			hands.myLeft = FlipRightToLeft(myFinalTransform);
			hands.myLeftHandState = EHandState::CLOSED;
		}
	}
	
	OverrideSwordData(hands, 0.8f, 1.0f);
	SetHands(hands);

	FFPAnimationCameraData camera;
	SetCamera(camera); 
	
	return nullptr;
}
