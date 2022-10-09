
#include "ArenaCompObject.h"

#include "Kismet/KismetMathLibrary.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Arena/ArenaGenerator.h"

void UArenaCompObject::Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision)
{
	CHECK_RETURN(!aArenaGenerator);

	const float height = ULevelRand::FRandRange(myMinHeight, myMaxHeight);
	const float anglePart = ULevelRand::FRandRange(myMinAnglePart, myMaxAnglePart);
	const float radiusPart = ULevelRand::FRandRange(myMinRadiusPart, myMaxRadiusPart);
	const FVector2D loc2D = GetSectionLocation(aLayer, aSubdivision, anglePart, radiusPart, false);
	const FVector loc = FVector(loc2D.X, loc2D.Y, aSubdivision.height + height);
	const FQuat rot = UKismetMathLibrary::FindLookAtRotation(loc * FVector(1.0f, 1.0f, 0.0f), FVector::ZeroVector).Quaternion();
	const FQuat rotOffset = rot * myTransform.GetRotation();
	const FVector offset = rot.RotateVector(myTransform.GetLocation());
	if (AActor* actor = aArenaGenerator->SpawnGeneratedActor(myClass, FTransform(rotOffset, loc + offset)))
		actor->SetActorScale3D(myTransform.GetScale3D());
}
