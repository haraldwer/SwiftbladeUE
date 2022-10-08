#include "SectionCompKnifeWall.h"

#include "Kismet/KismetMathLibrary.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompKnifeWall::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	CHECK_RETURN(!aRoom.walls.Num());
	
	const int32 wallIndex = ULevelRand::RandRange(0, aRoom.walls.Num() - 1);
	const auto& verts = aRoom.walls[wallIndex].verts;

	CHECK_RETURN(verts.Num() < 2);

	// Try 3 times
	for (int i = 0; i < 3; i++)
	{
		// Place it in the middle of the wall
		const int32 vertIndex = FMath::FloorToInt32((verts.Num() - 1) * 0.5f);

		const FVector2D start = verts[vertIndex];
		const FVector2D end = verts[vertIndex + 1];

		// Wall has to be big enough
		if (FVector2D::DistSquared(start, end) < myMinWallSize * myMinWallSize)
			continue;
	
		const FVector2D middle = (start + end) * 0.5f;

		const float height = aRoom.groundOffset + ULevelRand::FRandRange(100.0f, aRoom.ceilHeight - 100.0f);

		const FRotator rotation =
			UKismetMathLibrary::FindLookAtRotation(
				FVector(middle.X, middle.Y, 0.0f),
				FVector(aRoom.location.X, aRoom.location.Y, 0.0f));
		const FVector location = FVector(middle.X, middle.Y, height) + rotation.Vector().GetSafeNormal() * 20.0f;
		const FTransform trans = FTransform(rotation - FRotator(0.0f, 90.0f, 0.0f), location);
		aGenerator->SpawnGeneratedActor(myKnifeClass, trans);
		break;
	}
}
