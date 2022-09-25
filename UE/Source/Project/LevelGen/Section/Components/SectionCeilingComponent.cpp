#include "SectionCeilingComponent.h"
#include "Project/LevelGen/Section/FaceMeshGeneration.h"

void USectionCeilingComponent::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	const FVector offset = FVector(0, 0, aRoom.groundOffset + aRoom.ceilHeight + myCeilingThickness);
	CreateFaceMesh(aGenerator, aRoom, offset, myCeilingThickness, myCeilingMaterial);
}
