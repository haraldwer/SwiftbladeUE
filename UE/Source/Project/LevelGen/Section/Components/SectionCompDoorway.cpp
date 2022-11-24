#include "SectionCompDoorway.h"

#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompDoorway::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection,const FProcRoom& aRoom)
{
	for (auto& edge : aRoom.edges)
	{
		if (edge.isPath)
		{
			const auto& firstVert = aRoom.vertices[edge.startIndex];
			const auto& secondVert = aRoom.vertices[edge.endIndex];
			const FVector firstLoc = FVector(firstVert.X, firstVert.Y, aRoom.groundOffset + aRoom.ceilHeight * myCeilPart);
			const FVector secondLoc = FVector(secondVert.X, secondVert.Y, aRoom.groundOffset + aRoom.ceilHeight * myCeilPart);
			const FVector2D dir = -(firstVert - secondVert).GetSafeNormal();
			const float rot = FMath::RadiansToDegrees(FMath::Atan2(dir.Y, dir.X));
			PlaceObject(aGenerator, FTransform(FRotator(0.0f, rot, 0.0f), firstLoc), aRoom);
			PlaceObject(aGenerator, FTransform(FRotator(0.0f, rot + 180.0f, 0.0f), secondLoc), aRoom);
		}
	}
}

void USectionCompDoorway::PlaceObject(ASectionGenerator* aGenerator, const FTransform& aTransform, const FProcRoom& aRoom)
{
	if (const auto actor = aGenerator->SpawnGeneratedActor(myObjectType, aTransform))
	{
		if (myScaleHeight)
		{
			FVector origin;
			FVector extent;
			actor->GetActorBounds(false, origin, extent);
			const float height = extent.Z * 2.0f;
			actor->SetActorScale3D(FVector(1.0f, 1.0f, (1.0f / height) * aRoom.ceilHeight));
		}
	}
}
