#include "SectionCompCeiling.h"
#include "Project/LevelGen/Section/FaceMeshGeneration.h"

void USectionCompCeiling::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	const FVector offset = FVector(0, 0, aRoom.groundOffset + aRoom.ceilHeight + myCeilingThickness);
	CreateFaceMesh(aGenerator, aRoom, offset, myCeilingThickness, myCeilingMaterial);
}
