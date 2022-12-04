#include "Rail.h"

#include "Components/SplineComponent.h"

URail::URail()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URail::BeginPlay()
{
	Super::BeginPlay();

	const auto owner = GetOwner();
	CHECK_RETURN(!owner);
	auto comps = owner->GetComponents();
	for (const auto comp : comps)
		if (const auto spline = Cast<USplineComponent>(comp))
			mySpline = spline;
}
