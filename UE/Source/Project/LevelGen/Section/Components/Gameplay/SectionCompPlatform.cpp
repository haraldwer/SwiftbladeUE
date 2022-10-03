#include "SectionCompPlatform.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompPlatform::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	TArray<FVector2D> pathLocations;
	for (const auto& it : aRoom.edges)
		if (it.isPath)
			pathLocations.Add(it.location);

	const int32 centerIndex = pathLocations.Num() ? 1 : 0;
	pathLocations.Insert(aRoom.center, centerIndex);

	CHECK_RETURN(pathLocations.Num() < 2);
	
	const float part = 1.0f / static_cast<float>(myCount + 2);
	for (int32 i = 0; i < myCount; i++)
	{
		const float progress = (i + 1) * part;
		const float index = progress * pathLocations.Num();
		const int32 firstPathIndex = FMath::CeilToInt(index);
		const int32 secondPathindex = FMath::FloorToInt(index);
		
		const FVector2D firstPathLoc = pathLocations[firstPathIndex];
		const FVector2D secondPathLoc = pathLocations[secondPathindex];
		const float alpha = FMath::Fmod(index, 1.0f);
		const FVector2D pathLoc = FMath::Lerp(firstPathLoc, secondPathLoc, alpha);

		const float height = aRoom.groundOffset + FMath::RandRange(myMinHeight, myMaxHeight);
		const FVector location = FVector(pathLoc.X, pathLoc.Y, height);
		aGenerator->SpawnGeneratedActor(myPlatformClass, FTransform(location));
	}
}
