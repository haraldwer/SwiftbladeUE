#include "FPAnimationStateBlock.h"

UClass* UFPAnimationStateBlock::Update(float aDT)
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
	
	OverrideSwordData(hands, 0.8f, 1.0f, true);
	SetHands(hands);

	FFPAnimationCameraData camera;
	SetCamera(camera); 
	
	return nullptr;
}

void UFPAnimationStateBlock::Enter()
{
	Super::Enter();
}
