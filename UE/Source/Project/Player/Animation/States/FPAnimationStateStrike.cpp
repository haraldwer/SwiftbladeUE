#include "FPAnimationStateStrike.h"

#include "Curves/CurveFloat.h"

void UFPAnimationStateStrike::Init()
{
	Super::Init();

	// Get children
	TArray<USceneComponent*> children;
	GetChildrenComponents(false, children);
	for (const auto child : children)
	{
		TArray<USceneComponent*> transforms;
		child->GetChildrenComponents(false, transforms);
		CHECK_CONTINUE_LOG(transforms.Num() < 2, "Too few transforms on child");
		
		// Sort by name
		transforms.Sort([](const USceneComponent& aFirst, const USceneComponent& aSecond) {
			return aFirst.GetName() < aSecond.GetName(); });
		
		myEntries.Add({
			transforms[0]->GetRelativeTransform(),
			transforms[1]->GetRelativeTransform()
		});
	}
}

UClass* UFPAnimationStateStrike::Update(float aDT)
{
	Super::Update(aDT);

	const float timeDiff = GetCurrentTime() - myStrikeTimestamp; 
	const float timePart = FMath::Clamp(timeDiff / myStrikeDuration, 0.0f, 1.0f);
	
	auto trans = GetDefaultHandTransform();

	// Get entry trans
	if (myEntries.IsValidIndex(myAnimIndex))
	{
		const auto& entry = myEntries[myAnimIndex];
		const float weight =
			FMath::Clamp(myCurve ? myCurve->GetFloatValue(timePart) : timePart, 0.0f, 1.0f);
		trans = LerpTransWeight(
			entry.myStart,
			entry.myEnd,
			weight,
			weight);
	}
	
	// Lerp to camera
	const auto lerpTrans = LerpTransWeight(
		trans,
		ToCameraSpace(trans),
		0.8f, 0.8f);
	
	FFPAnimationHandPositions hands;
	hands.myRight = lerpTrans;
	
	OverrideSwordData(hands, 0.0f, 0.0f, true);
	SetHands(hands, true);

	FFPAnimationCameraData camera;
	SetCamera(camera); 
	
	return nullptr;
}

void UFPAnimationStateStrike::Enter()
{
	Super::Enter();
	myStrikeTimestamp = GetCurrentTime();

	// Randomize index
	const int32 num = myEntries.Num();
	myAnimIndex = FMath::RandRange(0, num - 1);

	// Prevent previous anim from being reused
	if (myEntries.Num() > 1)
		while (myAnimIndex == myPreviousIndex)
			myAnimIndex = (myAnimIndex + 1) % num;	
	myPreviousIndex = myAnimIndex;
}
