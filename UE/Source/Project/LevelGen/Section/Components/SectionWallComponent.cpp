#include "SectionWallComponent.h"

void USectionWallComponent::Populate(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	for (auto& room : aSection.rooms)
	{
		// Create wall
		const int32 numWalls = FMath::RandRange(myMinNumWalls, myMaxNumWalls);
		if (numWalls >= 0)
		{
			TArray<int32> wallArr;
			for (int32 index = 0; index < room.walls.Num(); index++)
				wallArr.Add(index);
			int32 wallCount = 0;
			while (wallCount < numWalls && wallArr.Num() > 0)
			{
				const int32 wallIndex = FMath::RandRange(0, wallArr.Num() - 1);
				const int32 index = wallArr[wallIndex];
				wallArr.RemoveAtSwap(wallIndex);
				CreateWall(aGenerator, room.walls[index].verts, wallCount, room.groundOffset, room.ceilHeight);
				wallCount++;
			}
		}
		else
		{
			for (int32 index = 0; index < room.walls.Num(); index++)
				if (room.walls[index].verts.Num() > 1)
					CreateWall(aGenerator, room.walls[index].verts, index, room.groundOffset, room.ceilHeight);	
		}
	}
}
