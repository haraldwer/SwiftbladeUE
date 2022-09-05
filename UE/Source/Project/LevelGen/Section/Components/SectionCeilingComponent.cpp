#include "SectionCeilingComponent.h"
#include "Project/LevelGen/Section/FaceMeshGeneration.h"

void USectionCeilingComponent::Populate(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	if (!myHasCeil)
		return;
	
	for (auto& room : aSection.rooms)
	{
		const FVector offset = FVector(0, 0, room.groundOffset + room.ceilHeight + myCeilingThickness);
		CreateFaceMesh(aGenerator, room, offset, myCeilingThickness, myCeilingMaterial);
	}
}
