#include "SectionCompBase.h"

#include "DrawDebugHelpers.h"
#include "Project/Utility/Math/LineIntersection.h"

TArray<int32> USectionCompBase::PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	const int32 numRooms = aSection.rooms.Num();

	TArray<int32> pool;
	for (int32 i = 0; i < numRooms; i++)
		pool.Add(i);

	if (myComponentChance > 99.0f)
		return pool; 
	
	const int32 numPopulatedRooms = FMath::Min(FMath::CeilToInt(numRooms * (myComponentChance / 100.0f)), numRooms);
	TArray<int32> populateRooms;
	for (int32 i = 0; i < numPopulatedRooms; i++)
	{
		const int32 randI = FMath::RandRange(0, pool.Num() - 1);
		const int32 index = pool[randI];
		pool.RemoveAtSwap(randI);
		populateRooms.Add(index);
	}
	
	return populateRooms;
}

void USectionCompBase::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	BPPopulateRoom(aGenerator, aSection, aRoom);
}


void USectionCompBase::GetRandomCrossSection(const FProcRoom& aRoom, FVector2D& aFirst, FVector2D& aSecond) const
{
	const int32 numWalls = aRoom.walls.Num();
	const int32 numVerts = aRoom.vertices.Num();
	
	// If few walls, just choose random location
	if (numWalls <= 1)
	{
		const int32 firstI = FMath::RandRange(0, numVerts - 1);
		const int32 secondI = (firstI + FMath::RandRange(1, numVerts - 1)) % (numVerts);
		aFirst = aRoom.vertices[firstI];
		aSecond = aRoom.vertices[secondI];
		return;
	}

	constexpr float padding = 0.5f;
	
	// Get first location
	const int32 firstWallI = FMath::RandRange(0, numWalls - 1);
	const auto& firstVerts = aRoom.walls[firstWallI].verts;
	if (firstVerts.Num())
	{
		const float index = FMath::RandRange(padding, firstVerts.Num() - 1.0f - padding);
		aFirst = GetBlendVert(firstVerts, index);	
	}
	else
	{
		aFirst = aRoom.vertices[FMath::RandRange(0, numVerts - 1)];
	}
	
	// Get second location
	const int32 secondWallI = (firstWallI + FMath::RandRange(1, numWalls - 1)) % (numWalls);
	const auto& secondVerts = aRoom.walls[secondWallI].verts;
	if (secondVerts.Num())
	{
		const float index = FMath::RandRange(padding, secondVerts.Num() - 1.0f - padding);
		aSecond = GetBlendVert(secondVerts, index);	
	}
	else
	{
		aSecond = aRoom.vertices[FMath::RandRange(0, numVerts - 1)];
	}
}
