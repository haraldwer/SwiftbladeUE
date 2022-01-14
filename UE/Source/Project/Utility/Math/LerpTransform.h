#pragma once

inline FTransform LerpTransWeight(const FTransform& aFirst, const FTransform& aSecond, float aLocationWeight, float aRotationWeight)
{
	const FVector location = FMath::Lerp(aFirst.GetLocation(), aSecond.GetLocation(), aLocationWeight);
	const FQuat rotation = FQuat::Slerp(aFirst.GetRotation(), aSecond.GetRotation(), aRotationWeight).GetNormalized();
	return FTransform(rotation, location);
}

inline FTransform LerpTransDelta(const FTransform& aFirst, const FTransform& aSecond, float aDT, float aSmoothing)
{
	const auto location = FMath::VInterpTo(aFirst.GetLocation(), aSecond.GetLocation(), aDT, aSmoothing);
	const auto rotation = FMath::QInterpTo(aFirst.GetRotation(), aSecond.GetRotation(), aDT, aSmoothing);
	return FTransform(rotation, location);
}