#include "SectionCompEnd.h"

#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Section/SectionEnd.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

TArray<int32> USectionCompEnd::PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	if (!aSection.rooms.Num())
		return {};

	return { aSection.rooms.Last().index };
}

void USectionCompEnd::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	auto& lastRoom = aSection.rooms.Last();
	const FTransform trans = FTransform(
		FVector(
			aSection.lastEdgeLoc.X,
			aSection.lastEdgeLoc.Y,
			lastRoom.groundOffset));

	const auto endIndex = ULevelRand::RandRange(0, myEnds.Num() - 1);
	CHECK_RETURN_LOG(!myEnds.IsValidIndex(endIndex), "Invalid index");
	const auto end = Cast<ASectionEnd>(aGenerator->SpawnGeneratedActor(myEnds[endIndex], trans));
	CHECK_RETURN_LOG(!end, "End nullptr");
	end->OnLocationSet();
}
