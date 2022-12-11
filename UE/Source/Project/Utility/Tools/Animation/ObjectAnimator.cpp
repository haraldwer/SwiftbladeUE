#include "ObjectAnimator.h"

#include "Utility/Math/LerpTransform.h"

UObjectAnimator::UObjectAnimator()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UObjectAnimator::BeginPlay()
{
	Super::BeginPlay();

	if (const auto base = Cast<USceneComponent>(GetOwner()->GetComponentByClass(USceneComponent::StaticClass())))
	{
		TArray<USceneComponent*> children;
		base->GetChildrenComponents(false, children);
		if (children.IsValidIndex(0))
			children[0]->GetChildrenComponents(true, myObjects);
		if (children.IsValidIndex(1))
			children[1]->GetChildrenComponents(true, myTransforms);
	}

	myObjects.Sort([](const USceneComponent& aFirst, const USceneComponent& aSecond) { return aFirst.GetName() < aSecond.GetName(); });
	myTransforms.Sort([](const USceneComponent& aFirst, const USceneComponent& aSecond) { return aFirst.GetName() < aSecond.GetName(); });
}

void UObjectAnimator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CHECK_RETURN(myDone);

	const float frameDelta = 1.0f / myFramerate; 
	myFrameTimer += DeltaTime;
	while (myFrameTimer > frameDelta)
	{
		myFrameTimer -= frameDelta;
		Update(frameDelta * mySpeedMultiplier);
	}
}

void UObjectAnimator::Update(const float aDT)
{
	CHECK_RETURN(!myKeys.Num());

	myDone = true;
	for (const auto& it : myKeys)
	{
		CHECK_CONTINUE(it.myObjectIndex == -1);
		CHECK_CONTINUE(it.myTransformIndex == -1);
		CHECK_CONTINUE(!myObjects.IsValidIndex(it.myObjectIndex));
		CHECK_CONTINUE(!myTransforms.IsValidIndex(it.myTransformIndex));
		if (!MoveObject(myObjects[it.myObjectIndex], myTransforms[it.myTransformIndex], it.mySpeed, it.myHidden, aDT))
			myDone = false;
	}	
}

void UObjectAnimator::SetKeys(const TArray<FObjectAnimatorKey>& someKeys)
{
	myKeys = someKeys;
	myDone = false;
}

bool UObjectAnimator::MoveObject(USceneComponent* anObject, USceneComponent* aTarget, const float aSpeed, const bool aHidden, const float aDT) const
{
	CHECK_RETURN_LOG(!anObject, "Invalid object", false);
	CHECK_RETURN_LOG(!aTarget, "Invalid target", false);
	
	anObject->SetVisibility(!aHidden, true);
	
	const auto& objTrans = anObject->GetRelativeTransform();
	const auto& tarTrans = aTarget->GetRelativeTransform();

	if (FVector::Dist(objTrans.GetLocation(), tarTrans.GetLocation()) < myMinDist &&
		objTrans.GetRotation().AngularDistance(tarTrans.GetRotation()) < myMinRot)
	{
		// Has reached destination
		anObject->SetRelativeTransform(tarTrans);
		return true;
	}
	
	const auto result = LerpTransDelta(
		objTrans,
		tarTrans,
		aDT,
		aSpeed,
		aSpeed);
	anObject->SetRelativeTransform(result);
	return false;
}

