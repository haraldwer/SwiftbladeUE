#include "SectionCompSpikeLine.h"

#include "Kismet/KismetMathLibrary.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompSpikeLine::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	FVector2D first;
	FVector2D second;
	if (!GetRandomCrossSection(aRoom, first, second))
		return;
		
	const FVector2D middle = (first + second) * 0.5f;
	const FRotator rot = UKismetMathLibrary::FindLookAtRotation(
		FVector(first.X, first.Y, 0.0f),
		FVector(second.X, second.Y, 0.0f));
	const FVector loc = FVector(middle.X, middle.Y, aRoom.groundOffset);
	const float scale = FVector2D::Distance(first, second) * (0.5f / 100.f);
	const FTransform trans = FTransform(rot, loc);
		
	if (AActor* actor = aGenerator->SpawnGeneratedActor(mySpikeClass, trans))
	{
		actor->SetActorRelativeScale3D(FVector(scale, 1.0f, 1.0f));
		actor->RerunConstructionScripts(); 			
	}
}
