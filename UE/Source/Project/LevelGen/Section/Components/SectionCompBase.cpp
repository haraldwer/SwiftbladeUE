#include "SectionCompBase.h"

#include "DrawDebugHelpers.h"
#include "Project/LevelGen/LevelRand.h"
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
		const int32 randI = ULevelRand::RandRange(0, pool.Num() - 1);
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

bool USectionCompBase::GetRandomCrossSection(const FProcRoom& aRoom, FVector2D& aFirst, FVector2D& aSecond)
{
	const int32 numWalls = aRoom.walls.Num();
	const int32 numVerts = aRoom.vertices.Num();

	constexpr float padding = 0.2f;
	
	const int32 firstWallI = ULevelRand::RandRange(0, numWalls - 1);
	const auto& firstVerts = aRoom.walls[firstWallI].verts;
	
	const int32 secondWallI = (firstWallI + ULevelRand::RandRange(1, numWalls - 1)) % numWalls;
	const auto& secondVerts = aRoom.walls[secondWallI].verts;
	
	if (firstVerts.Num() && secondVerts.Num() && firstWallI != secondWallI)
	{
		const float firstIndex = ULevelRand::FRandRange(padding, firstVerts.Num() - 1.0f - padding);
		aFirst = GetBlendVert(firstVerts, firstIndex);
		
		const float secondIndex = ULevelRand::FRandRange(padding, secondVerts.Num() - 1.0f - padding);
		aSecond = GetBlendVert(secondVerts, secondIndex);

		return true;
	}

	if (aRoom.vertices.Num() < 2)
		return false; 
	
	// If few walls, just choose random location
	const float firstI = ULevelRand::FRandRange(0.0f, numVerts - 1.0f);
	const float secondI = ULevelRand::FRandRange(0.0f, numVerts - 1.0f);
	aFirst = GetBlendVert(aRoom.vertices, firstI); 
	aSecond = GetBlendVert(aRoom.vertices, secondI);

	return false;
}
