#include "ObjectAnimator.h"

#include "Project/Utility/Math/LerpTransform.h"

UObjectAnimator::UObjectAnimator()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UObjectAnimator::BeginPlay()
{
	Super::BeginPlay();

	auto base = Cast<USceneComponent>(GetOwner()->GetComponentByClass(USceneComponent::StaticClass()));
	if (base)
	{
		TArray<USceneComponent*> children;
		base->GetChildrenComponents(false, children);
		if (children.IsValidIndex(0))
			myObjectParent = children[0];
		if (children.IsValidIndex(1))
			myTransformParent = children[1];
	}
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

	CHECK_RETURN(!myObjectParent.IsValid());
	CHECK_RETURN(!myTransformParent.IsValid());

	TArray<USceneComponent*> objects;
	myObjectParent->GetChildrenComponents(true, objects);
	TArray<USceneComponent*> transforms;
	myTransformParent->GetChildrenComponents(true, transforms);

	myDone = true;
	for (const auto& it : myKeys)
	{
		CHECK_CONTINUE(it.myObjectIndex == -1);
		CHECK_CONTINUE(it.myTransformIndex == -1);
		CHECK_CONTINUE(it.myObjectIndex < 0 || it.myObjectIndex >= objects.Num());
		CHECK_CONTINUE(it.myTransformIndex < 0 || it.myTransformIndex >= transforms.Num());
		if (!MoveObject(objects[it.myObjectIndex], transforms[it.myTransformIndex], it.mySpeed, aDT))
			myDone = false;
	}	
}

void UObjectAnimator::SetKeys(const TArray<FObjectAnimatorKey>& someKeys)
{
	myKeys = someKeys;
	myDone = false;
}

bool UObjectAnimator::MoveObject(USceneComponent* anObject, USceneComponent* aTarget, float aSpeed, float aDT)
{
	CHECK_RETURN_LOG(!anObject, "Invalid object", false);
	CHECK_RETURN_LOG(!aTarget, "Invalid target", false);

	const auto& objTrans = anObject->GetRelativeTransform();
	const auto& tarTrans = aTarget->GetRelativeTransform();

	if (FVector::Dist(objTrans.GetLocation(), tarTrans.GetLocation()) < myMinDist &&
		objTrans.GetRotation().AngularDistance(tarTrans.GetRotation()) < myMinRot)
	{
		// Has reached destination
		anObject->SetRelativeTransform(tarTrans);
		return true;
	}
	
	const auto result = LerpTransDelta( objTrans, tarTrans, aDT, aSpeed);
	anObject->SetRelativeTransform(result);
	return false;
}

