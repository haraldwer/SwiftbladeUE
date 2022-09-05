#include "SectionGroundComponent.h"
#include "Project/LevelGen/Section/FaceMeshGeneration.h"

void USectionGroundComponent::Populate(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	if (!myHasGround)
		return;
	
	for (auto& room : aSection.rooms)
	{
		const FVector offset = FVector(0, 0, room.groundOffset);
		CreateFaceMesh(aGenerator, room, offset, myGroundThickness, myGroundMaterial);
	}
}
