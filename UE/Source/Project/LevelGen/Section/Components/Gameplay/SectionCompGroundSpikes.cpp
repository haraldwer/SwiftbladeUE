#include "SectionCompGroundSpikes.h"

#include "Kismet/KismetMathLibrary.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompGroundSpikes::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	FVector2D first;
	FVector2D second;
	GetRandomCrossSection(aRoom, first, second);

	const FRotator rot = UKismetMathLibrary::FindLookAtRotation(
		FVector(first.X, first.Y, 0.0f),
		FVector(second.X, second.Y, 0.0f));

	const FVector right = FRotator(rot.Pitch, rot.Yaw + 90.0f, rot.Roll).Vector();
	
	const float dist = FVector2D::Distance(first, second);
	const int32 num = static_cast<int32>(dist / mySize);
	for (int32 i = 0; i < num; i++)
	{
		const float part = (static_cast<float>(i) + 0.5f) / static_cast<float>(num);
		const FVector2D lerp = FMath::Lerp(first, second, part);
		const FVector2D offset = FVector2D(right.X, right.Y) * myUseAlternatingOffset * (i % 2) * mySize;
		const FVector2D loc = lerp + offset;
		const FVector final = FVector(loc.X, loc.Y, aRoom.groundOffset);
		const FTransform trans = FTransform(rot, final);
		aGenerator->SpawnGeneratedActor(mySpikeClass, trans);
	}
}
