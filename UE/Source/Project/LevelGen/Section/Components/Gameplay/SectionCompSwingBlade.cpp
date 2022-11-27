#include "SectionCompSwingBlade.h"

#include "Kismet/KismetMathLibrary.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompSwingBlade::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	FVector2D first;
	FVector2D second;
	GetRandomCrossSection(aRoom, first, second); 

	// Lerp middle
	const float dist = FVector2D::Distance(first, second);
	const float randDist = dist * 0.5f > myWallDist ?
		ULevelRand::FRandRange(myWallDist, dist - myWallDist) :
		dist * 0.5f; // Middle if too tight
	const float part = randDist / dist;
	const FVector2D lerp = FMath::Lerp(first, second, part);

	// Height?
	const float height = FMath::Max(aRoom.ceilHeight, myMinHeight);
	const FVector loc = FVector(lerp.X, lerp.Y, aRoom.groundOffset + height);
	
	const FRotator rot = UKismetMathLibrary::FindLookAtRotation(
		FVector(first.X, first.Y, 0.0f),
		FVector(second.X, second.Y, 0.0f));
	
	const FTransform trans = FTransform(rot, loc);
	aGenerator->SpawnGeneratedActor(myBladeClass, trans);
}
