#include "RailComponent.h"

#include "Components/SplineComponent.h"

URailComponent::URailComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URailComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto owner = GetOwner();
	CHECK_RETURN(!owner);
	auto comps = owner->GetComponents();
	for (const auto comp : comps)
		if (const auto spline = Cast<USplineComponent>(comp))
			mySpline = spline;
}
