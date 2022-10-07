
#include "ArenaCompWall.h"

#include "Project/LevelGen/Arena/ArenaGenerator.h"
#include "Project/LevelGen/Section/FaceMeshGeneration.h"

void UArenaCompWall::Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision)
{
	const int32 numVerts = mySubdivideWall ? myWallSubdivisions : 2; 
	TArray<FVector2D> wallVerts;
	for (int i = 0; i < numVerts; i++)
	{
		float part = i * (1.0f / static_cast<float>(numVerts - 1));
		const float anglePart = FMath::Lerp(myAnglePadding, 1.0f - myAnglePadding, part);
		wallVerts.Add(
			GetSectionLocation(
				aLayer,
				aSubdivision,
				anglePart,
				myRadiusPart));
	}
	
	const float groundOffset = FMath::RandRange(myMinWallGroundOffset, myMaxWallGroundOffset) + aSubdivision.height;
	const float height = FMath::RandRange(myMinWallHeight, myMaxWallHeight);
	CreateWall(aArenaGenerator, wallVerts, groundOffset, height);
}
