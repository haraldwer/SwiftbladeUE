#include "SectionCompWall.h"

#include "Project/LevelGen/LevelRand.h"

void USectionCompWall::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	const float groundOffset = aRoom.groundOffset + myGroundOffset;
	const float height = myHeightOverride > SMALL_NUMBER ? myHeightOverride : aRoom.ceilHeight; 
	
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
			if (aRoom.walls[index].verts.Num() <= 1)
				continue;
			const auto verts = GetAdjustedVerts(aRoom.walls[index].verts, aRoom.center);
			CreateWall(
				aGenerator,
				verts,
				wallCount,
				groundOffset,
				height);
			wallCount++;
		}
	}
	else
	{
		for (int32 index = 0; index < aRoom.walls.Num(); index++)
		{
			if (aRoom.walls[index].verts.Num() <= 1)
				continue;
			const auto verts = GetAdjustedVerts(aRoom.walls[index].verts, aRoom.center);
			CreateWall(
				aGenerator,
				verts,
				index,
				groundOffset,
				height);
		}	
	}
}

TArray<FVector2D> USectionCompWall::GetAdjustedVerts(const TArray<FVector2D>& someVerts, const FVector2D& aCenter) const
{
	TArray<FVector2D> verts = someVerts;
	
	if (myWallDist > SMALL_NUMBER)
	{
		for (auto& vert : verts)
			vert -= (vert - aCenter).GetSafeNormal() * myWallDist;
	}

	if (myIndexPadding > SMALL_NUMBER && verts.Num() >= 2)
	{
		const float startPadding = myIndexPadding;
		const float endPadding = verts.Num() - 1.0f - myIndexPadding;
		verts[0] = GetBlendVert(verts, startPadding);
		verts[verts.Num() - 1] = GetBlendVert(verts, endPadding); 
	}
	
	return verts; 
}
