#include "SectionGroundComponent.h"
#include "Project/LevelGen/Section/FaceMeshGeneration.h"

void USectionGroundComponent::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection,const FProcRoom& aRoom)
{
	const FVector offset = FVector(0, 0, aRoom.groundOffset);
	CreateFaceMesh(aGenerator, aRoom, offset, myGroundThickness, myGroundMaterial);
}
