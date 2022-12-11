#pragma once

inline bool LineIntersect(
		const FVector2D& aSegmentStartA, const FVector2D& aSegmentEndA,
		const FVector2D& aSegmentStartB, const FVector2D& aSegmentEndB,
		FVector2D& anIntersect)
{
	const FVector2D vectorA = aSegmentEndA - aSegmentStartA;
	const FVector2D vectorB = aSegmentEndB - aSegmentStartB;

	const FVector::FReal s = (-vectorA.Y * (aSegmentStartA.X - aSegmentStartB.X) + vectorA.X *
		(aSegmentStartA.Y - aSegmentStartB.Y)) / (-vectorB.X * vectorA.Y + vectorA.X * vectorB.Y);
	const FVector::FReal t = (vectorB.X * (aSegmentStartA.Y - aSegmentStartB.Y) - vectorB.Y *
		(aSegmentStartA.X - aSegmentStartB.X)) / (-vectorB.X * vectorA.Y + vectorA.X * vectorB.Y);

	const bool bIntersects = (s >= 0 && s <= 1 && t >= 0 && t <= 1);

	if (bIntersects)
	{
		anIntersect.X = aSegmentStartA.X + (t * vectorA.X);
		anIntersect.Y = aSegmentStartA.Y + (t * vectorA.Y);
	}

	return bIntersects;
};