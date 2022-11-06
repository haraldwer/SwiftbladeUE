#include "SectionCompGround.h"
#include "Project/ProcGen/FaceMeshGeneration.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompGround::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection,const FProcRoom& aRoom)
{
	const FVector offset = FVector(0, 0, aRoom.groundOffset);
	CreateFaceMesh(aGenerator, aRoom.vertices, offset, myGroundThickness, myGroundMaterial);

	Super::PopulateRoom(aGenerator, aSection, aRoom);
}
