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

	const FTransform defaultTrans = GetDefaultHandTransform();
	
	// Lerp to camera   
	const auto lerpTrans = LerpTransWeight(
		defaultTrans,
		ToCameraSpace(defaultTrans),
		0.7f, 0.7f);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	
	hands.myPosInterpSpd = 10.0f;
	hands.myRotInterpSpd = 10.0f;

	OverrideSwordData(hands, 0.0f, 0.0f, true);
	OverrideVelocityData(hands, 0.1f);
	SetHands(hands, false);

	if (GetStateTime() > myDuration)
		return UFPAnimationStateIdle::StaticClass();
	
	return nullptr;
}
