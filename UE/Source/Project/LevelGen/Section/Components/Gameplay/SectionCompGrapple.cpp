#include "SectionCompGrapple.h"
#include "../SectionCompCeiling.h"
#include "Project/Gameplay/GrapplePoint.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionCompGrapple::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	if (GetRoomComp<USectionCompCeiling>(aRoom))
	{
		// Indoor grapple
	}
	else
	{
		float maxY = -1.0f;
		FProcEdge const* lastEdge = nullptr;
		for (auto& edge : aRoom.edges)
		{
			if (edge.isPath && edge.normal.Y > maxY)
			{
				lastEdge = &edge;
				maxY = edge.normal.Y;
			}
		}

		if (lastEdge)
		{
			// Outdoor grapple
			const FVector2D edgeLoc = lastEdge->location;
			const FVector2D lerpLoc = FMath::Lerp(aRoom.center, edgeLoc, 0.8f);
			const float height = aRoom.groundOffset + aRoom.ceilHeight + myHeight;
			const FVector location = FVector(lerpLoc.X, lerpLoc.Y, height);
			const FRotator rotation = FRotator(90.0f, 0.0f, 0.0f);
			const FTransform trans = FTransform(rotation, location);
			aGenerator->SpawnGeneratedActor(myGrappleClass, trans);
		}
	}
}
