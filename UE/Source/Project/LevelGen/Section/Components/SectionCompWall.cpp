#include "SectionCompWall.h"

#include "Project/LevelGen/LevelRand.h"

void USectionCompWall::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	// Create wall
	const int32 numWalls = ULevelRand::RandRange(myMinNumWalls, myMaxNumWalls);
	if (numWalls >= 0)
	{
		TArray<int32> wallArr;
		for (int32 index = 0; index < aRoom.walls.Num(); index++)
			wallArr.Add(index);
		int32 wallCount = 0;
		while (wallCount < numWalls && wallArr.Num() > 0)
		{
			const int32 wallIndex = ULevelRand::RandRange(0, wallArr.Num() - 1);
			const int32 index = wallArr[wallIndex];
			wallArr.RemoveAtSwap(wallIndex);
			const auto verts = GetAdjustedVerts(aRoom.walls[index].verts, aRoom.center);
			CreateWall(aGenerator, verts, wallCount, aRoom.groundOffset, aRoom.ceilHeight);
			wallCount++;
		}
	}
	else
	{
		for (int32 index = 0; index < aRoom.walls.Num(); index++)
			if (aRoom.walls[index].verts.Num() > 1)
				CreateWall(
					aGenerator,
					GetAdjustedVerts(aRoom.walls[index].verts, aRoom.center),
					index,
					aRoom.groundOffset,
					aRoom.ceilHeight);	
	}
}

TArray<FVector2D> USectionCompWall::GetAdjustedVerts(const TArray<FVector2D>& someVerts, const FVector2D& aCenter) const
{
	if (myWallDist < SMALL_NUMBER)
		return someVerts;
	TArray<FVector2D> verts = someVerts;
	for (auto& vert : verts)
		vert -= (vert - aCenter).GetSafeNormal() * myWallDist;
	return verts; 
}
