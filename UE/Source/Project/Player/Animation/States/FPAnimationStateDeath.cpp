#include "FPAnimationStateDeath.h"

#include "FPAnimationStateIdle.h"
#include "Project/Player/FPTime.h"

void UFPAnimationStateDeath::Enter()
{
	Super::Enter();
	ResetStateTime(); 
}

UClass* UFPAnimationStateDeath::Update(const float aDT)
{
	Super::Update(aDT);

	const bool respawned = GetTime().GetDilationValue(EDilationType::DEATH) == -1.0f;

	FTransform defaultTrans = GetDefaultHandTransform();
	if (respawned)
	{
		if (const auto idleState = GetState<UFPAnimationStateIdle>())
		{
			const auto idleTrans = idleState->GetDefaultHandTransform();
			const float alpha = FMath::Clamp(GetStateTime() / myInitialLerpTime, 0.0f, 1.0f);
			defaultTrans = LerpTransWeight(
				idleTrans,
				defaultTrans,
				alpha,
				alpha);
		}
	}

	// Lerp to camera
	const float lerpWeight = respawned ? 0.8f : 1.0f;  
	const auto lerpTrans = LerpTransWeight(
		defaultTrans,
		ToCameraSpace(defaultTrans),
		lerpWeight, lerpWeight);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	hands.myLeft = GetRealHands().myLeft;
	
	hands.myPosInterpSpd = respawned ? 5.0f : 20.0f;
	hands.myRotInterpSpd = respawned ? 5.0f : 20.0f;

	OverrideSwordData(hands, 0.0f, 0.0f, false);
	OverrideVelocityData(hands, respawned ? 0.2f : 0.0f);
	SetHands(hands, false);

	if (GetStateTime() > myDuration)
		return UFPAnimationStateIdle::StaticClass();
	return nullptr;
}
