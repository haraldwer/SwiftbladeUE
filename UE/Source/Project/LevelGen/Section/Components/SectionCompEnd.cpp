#include "SectionCompEnd.h"
#include "Kismet/GameplayStatics.h"
#include "Project/LevelGen/Level/LevelEndLocation.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

TArray<int32> USectionCompEnd::PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	if (!aSection.rooms.Num())
		return {};

	auto& lastRoom = aSection.rooms.Last();
	if (const auto levelEnd = aGenerator->GetLevelEnd())
	{
		levelEnd->SetActorLocation(FVector(aSection.lastEdgeLoc.X, aSection.lastEdgeLoc.Y, lastRoom.groundOffset));
		levelEnd->OnEndLocationSet(aGenerator);
	}

	return {};
}
