#include "FPAnimationStateBlock.h"

void UFPAnimationStateBlock::Init()
{
	Super::Init();

	TArray<USceneComponent*> children;
	GetChildrenComponents(false, children);
	for (const auto& child : children)
		myEntries.Add(child);
}

UClass* UFPAnimationStateBlock::Update(float aDT)
{
	Super::Update(aDT);

	auto trans = GetDefaultHandTransform();

	// Use entry 
	if (myEntries.IsValidIndex(myAnimIndex))
	{
		if (const auto entry = myEntries[myAnimIndex].Get())
			trans = entry->GetComponentTransform();
	}
		
	// Lerp to camera
	const auto lerpTrans = LerpTransWeight(
		trans,
		ToCameraSpace(trans),
		0.5f, 0.5f);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	hands.myPosInterpSpd = 25.0f;
	hands.myRotInterpSpd = 25.0f;

	// Apply wobble
	hands.myRight.SetLocation(hands.myRight.GetLocation() + Get3DNoise(myHandWobbleSpeed, myHandWobbleStrength));
	
	OverrideSwordData(hands, 0.0f, 0.0f, true);
	OverrideVelocityData(hands, 0.5f, aDT);
	SetHands(hands);

	FFPAnimationCameraData camera;
	SetCamera(camera); 
	
	return nullptr;
}

void UFPAnimationStateBlock::Enter()
{
	Super::Enter();

	// Randomize index
	const int32 num = myEntries.Num();
	myAnimIndex = FMath::RandRange(0, num - 1);

	// Prevent previous anim from being reused
	if (myEntries.Num() > 1)
		while (myAnimIndex == myPreviousIndex)
			myAnimIndex = (myAnimIndex + 1) % num;	
	myPreviousIndex = myAnimIndex;
	
}

void UFPAnimationStateBlock::OnBlock() const
{
	const FVector offset = FVector(100, 0, 0);
	
	auto hands = GetRealHands();
	hands.myRight.SetLocation(hands.myRight.GetLocation() - offset);
	hands.myLeft.SetLocation(hands.myLeft.GetLocation() - offset);
	SetHands(hands, true);

	auto camera = GetRealCamera();
	camera.myTilt += FMath::RandRange(-1.0f, 1.0f);
	SetCamera(camera, true); 
}
