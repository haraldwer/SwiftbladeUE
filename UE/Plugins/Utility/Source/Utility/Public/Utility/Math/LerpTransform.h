#pragma once

inline FTransform LerpTransWeight(const FTransform& aFirst, const FTransform& aSecond, const float aLocationWeight, const float aRotationWeight)
{
	const FVector location = FMath::Lerp(aFirst.GetLocation(), aSecond.GetLocation(), aLocationWeight);
	const FQuat rotation = FQuat::Slerp(aFirst.GetRotation(), aSecond.GetRotation(), aRotationWeight).GetNormalized();
	return FTransform(rotation, location);
}

inline FTransform LerpTransDelta(const FTransform& aCurrent, const FTransform& aTarget, const float aDT, const float aRotSmoothing, const float aPosSmoothing, float aMaxDist = -1.0f)
{
	if (aMaxDist > 0.0f && FVector::DistSquared(aCurrent.GetLocation(), aTarget.GetLocation()) > aMaxDist * aMaxDist)
		return aTarget;
	const auto location = FMath::VInterpTo(aCurrent.GetLocation(), aTarget.GetLocation(), aDT, aPosSmoothing);
	const auto rotation = FMath::QInterpTo(aCurrent.GetRotation(), aTarget.GetRotation(), aDT, aRotSmoothing);
	return FTransform(rotation, location);
}