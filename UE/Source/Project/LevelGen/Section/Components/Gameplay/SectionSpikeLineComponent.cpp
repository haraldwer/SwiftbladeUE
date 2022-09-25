#include "SectionSpikeLineComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionSpikeLineComponent::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	FVector2D first;
	FVector2D second;

	if (aRoom.walls.Num() > 1)
	{
		const int32 firstWallI = FMath::RandRange(0, aRoom.walls.Num() - 1);
		int32 secondWallI = firstWallI;
		while (secondWallI == firstWallI) 
			secondWallI = FMath::RandRange(0, aRoom.walls.Num() - 1);
				
		const int32 firstI = FMath::RandRange(0, aRoom.walls[firstWallI].verts.Num() - 1);
		const int32 secondI = FMath::RandRange(0, aRoom.walls[secondWallI].verts.Num() - 1);

		if (!aRoom.walls[firstWallI].verts.Num())
			first = aRoom.vertices[FMath::RandRange(0, aRoom.vertices.Num() - 1)];
		else
			first = aRoom.walls[firstWallI].verts[firstI];

		if (!aRoom.walls[secondWallI].verts.Num())
			second = aRoom.vertices[FMath::RandRange(0, aRoom.vertices.Num() - 1)];
		else
			second = aRoom.walls[secondWallI].verts[secondI];
	}
	else
	{
		const int32 firstI = FMath::RandRange(0, aRoom.vertices.Num() - 1);
		const int32 secondI = FMath::RandRange(0, aRoom.vertices.Num() - 1);
		if (firstI == secondI)
			return;
		first = aRoom.vertices[firstI];
		second = aRoom.vertices[secondI];
	}
		
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
