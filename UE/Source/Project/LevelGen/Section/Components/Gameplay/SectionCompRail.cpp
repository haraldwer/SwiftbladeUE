#include "SectionCompRail.h"

#include "Project/LevelGen/LevelRand.h"

FVector USectionCompRail::GetOffset() const
{
	return Super::GetOffset() + FVector::UpVector * ULevelRand::FRandRange(-myRandHeight, myRandHeight);
}
