#include "SectionCompWall.h"

void USectionCompWall::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	// Create wall
	const int32 numWalls = FMath::RandRange(myMinNumWalls, myMaxNumWalls);
	if (numWalls >= 0)
	{
		TArray<int32> wallArr;
		for (int32 index = 0; index < aRoom.walls.Num(); index++)
			wallArr.Add(index);
		int32 wallCount = 0;
		while (wallCount < numWalls && wallArr.Num() > 0)
		{
			const int32 wallIndex = FMath::RandRange(0, wallArr.Num() - 1);
			const int32 index = wallArr[wallIndex];
			wallArr.RemoveAtSwap(wallIndex);
			CreateWall(aGenerator, aRoom.walls[index].verts, wallCount, aRoom.groundOffset, aRoom.ceilHeight);
			wallCount++;
		}
	}
	else
	{
		for (int32 index = 0; index < aRoom.walls.Num(); index++)
			if (aRoom.walls[index].verts.Num() > 1)
				CreateWall(aGenerator, aRoom.walls[index].verts, index, aRoom.groundOffset, aRoom.ceilHeight);	
	}
}
