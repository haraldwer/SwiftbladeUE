#include "FPAnimationStateGrapple.h"

#include "Curves/CurveFloat.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Actors/Hand.h"
#include "Project/Player/Effects/FPPostProcessing.h"
#include "Project/Player/Movement/FPMovement.h"
#include "Project/Player/Movement/States/FPMovementStateGrapple.h"
#include "Project/Utility/Tools/Effect.h"

void UFPAnimationStateGrapple::Enter()
{
	Super::Enter();
	ResetStateTime();
	myHasCreatedEffect = false; 
}

void UFPAnimationStateGrapple::Exit()
{
	Super::Exit();
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
		const FTransform actorTrans = GetActorTransform();
		const FVector direction = (grappleState->GetTargetLocation() - actorTrans.GetLocation()).GetSafeNormal();
		if (grappleState->GetIsInFreeze())
		{
			// Extend
			hands.myPosInterpSpd = 30.0f;
			hands.myRotInterpSpd = 30.0f;

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
		
		UpdateMagicEffects(direction);
	}
	
	OverrideSwordData(hands, 0.8f, 1.0f, false);
	SetHands(hands);

	FFPAnimationCameraData camera;
	SetCamera(camera);

	
	return nullptr;
}

void UFPAnimationStateGrapple::UpdateMagicEffects(const FVector& aDirection)
{
	auto& character = GetCharacter();
	const float time = GetStateTime();

	if (myMagicEffectCurve && false)
	{
		const float value = myMagicEffectCurve->GetFloatValue(time);
		if (const auto pp = character.GetPostProcessing())
			pp->SetPPScalar(PP_MAGIC, "Strength", value, true);
		if (const auto hand = character.GetLeftHand())
			hand->SetMagicStrength(value);
	}

	if (time > myEffectDelay && !myHasCreatedEffect)
	{
		myHasCreatedEffect = true;
		const FTransform trans = FTransform(aDirection.Rotation(), character.GetActorLocation());
		if (const auto effect = character.CreateEffect(myEffectType, trans))
			effect->AttachToActor(&character, FAttachmentTransformRules::KeepWorldTransform);
	}
}
