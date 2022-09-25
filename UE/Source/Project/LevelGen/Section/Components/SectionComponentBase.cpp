#include "SectionComponentBase.h"

TArray<int32> USectionComponentBase::PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	const int32 numRooms = aSection.rooms.Num();
	const int32 numPopulatedRooms = FMath::Min(FMath::CeilToInt(numRooms * (myRoomChance / 100.0f)), numRooms);
	
	TArray<int32> pool;
	for (int32 i = 0; i < numRooms; i++)
		pool.Add(i);

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

void USectionComponentBase::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	BPPopulateRoom(aGenerator, aSection, aRoom);
}
