
#include "ArenaCompGround.h"

#include "Project/LevelGen/Arena/ArenaGenerator.h"
#include "Project/LevelGen/Section/FaceMeshGeneration.h"

void UArenaCompGround::Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision)
{
	const FVector offset = FVector(0, 0, aSubdivision.height);
	CreateFaceMesh(aArenaGenerator, aSubdivision.vertices, offset, myGroundThickness, myGroundMaterial);
}
