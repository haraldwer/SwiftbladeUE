#include "SectionCompPlatform.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompPlatform::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	CHECK_RETURN_LOG(!myPlatformClasses.Num(), "No platform classes set");

	// Randomize class per room
	const int32 classIndex = FMath::RandRange(0, myPlatformClasses.Num() - 1);
	const auto& platformClass = myPlatformClasses[classIndex];
	const float height = FMath::RandRange(myMinHeight, myMaxHeight);
	
	const float part = 1.0f / static_cast<float>(myCount + 2);
	for (int32 i = 0; i < myCount; i++)
	{
		const float progress = (i + 1) * part;
		const float index = progress * aRoom.path.Num();
		const FVector2D pathLoc = GetBlendVert(aRoom.path, index);
		const FVector location = FVector(pathLoc.X, pathLoc.Y, aRoom.groundOffset + height);
		aGenerator->SpawnGeneratedActor(platformClass, FTransform(location));
	}
}
