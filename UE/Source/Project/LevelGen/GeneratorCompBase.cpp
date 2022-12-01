#include "GeneratorCompBase.h"

#include "Project/Utility/Math/LineIntersection.h"

FVector2D UGeneratorCompBase::GetBlendVert(const TArray<FVector2D>& someVerts, const float anIndex)
{
	if (someVerts.Num() == 1)
		return someVerts[0]; 
	const float clampedIndex = FMath::Clamp(anIndex, 0.0f, someVerts.Num());
	const int32 firstVertI = FMath::Clamp(FMath::FloorToInt(clampedIndex), 0, someVerts.Num() - 1);
	const int32 secondVertI = FMath::Clamp(FMath::CeilToInt(clampedIndex), 0, someVerts.Num() - 1);
	const float alpha = FMath::Fmod(clampedIndex, 1.0f);
	const FVector2D firstVert = someVerts[firstVertI];
	const FVector2D secondVert = someVerts[secondVertI];
	return FMath::Lerp(firstVert, secondVert, alpha);	
}

bool UGeneratorCompBase::FitIntoMesh(FVector2D& aPosition, const float aRadius, const TArray<FVector2D> aMesh,
	const FVector2D aMeshCenter, UWorld* aWorld)
{
	const int32 numVerts = aMesh.Num();
	CHECK_RETURN(!numVerts, false);

	const FVector2D& start = aMeshCenter;
	const FVector2D& end = aPosition;
	const FVector2D& dir = (end - start).GetSafeNormal();
	const FVector2D& adjustedEnd = end + dir * aRadius;
	
	bool changed = false;
	
	for (int i = 0; i < numVerts; i++)
	{
		const FVector2D& currVert = aMesh[i];
		const FVector2D& nextVert = aMesh[(i + 1) % (numVerts - 1)];
		if (LineIntersect(
			currVert,
			nextVert,
			start,
			adjustedEnd,
			aPosition))
		{	
			aPosition -= dir * aRadius;
			changed = true;
		}
	}
	
	return changed; 
}
