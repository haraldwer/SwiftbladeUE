#include "SectionCompCorner.h"

#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompCorner::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection,const FProcRoom& aRoom)
{
	TSet<int32> vertsToIgnore;
	if (myAvoidDoorways)
		for (auto& edge : aRoom.edges)
			if (edge.isPath)
				vertsToIgnore.Append({edge.startIndex, edge.endIndex});

	for (int32 vI = 0; vI < aRoom.vertices.Num(); vI++)
	{
		CHECK_CONTINUE(vertsToIgnore.Contains(vI));
		auto& vert = aRoom.vertices[vI]; 
		const FVector2D dir = -(vert - aRoom.center).GetSafeNormal();
		const float rot = FMath::RadiansToDegrees(FMath::Atan2(dir.Y, dir.X));
		const FVector2D locOff = vert + dir * myOffset;  
		const FVector loc = FVector(locOff.X, locOff.Y, aRoom.groundOffset + aRoom.ceilHeight * myCeilPart);		
		PlaceObject(aGenerator, FTransform(FRotator(0.0f, rot, 0.0f), loc), aRoom);
	}
}

void USectionCompCorner::PlaceObject(ASectionGenerator* aGenerator, const FTransform& aTransform, const FProcRoom& aRoom)
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
