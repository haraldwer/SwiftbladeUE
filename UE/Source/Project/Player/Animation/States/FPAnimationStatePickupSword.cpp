#include "FPAnimationStatePickupSword.h"

#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Combat/FPCombat.h"

UClass* UFPAnimationStatePickupSword::Update(float aDT)
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
	hands.myPosInterpSpd = 10.0f;
	hands.myRotInterpSpd = 10.0f;

	if (const auto sword = GetCombat().GetSword())
		hands.myRight.SetLocation(GetActorTransform().InverseTransformPosition(sword->GetActorLocation()));
	
	SetHands(hands);

	FFPAnimationCameraData camera;
	SetCamera(camera); 
	
	return nullptr;
}
