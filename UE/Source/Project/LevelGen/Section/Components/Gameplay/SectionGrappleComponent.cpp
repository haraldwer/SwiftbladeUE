#include "SectionGrappleComponent.h"
#include "../SectionCeilingComponent.h"
#include "Project/Gameplay/GrapplePoint.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionGrappleComponent::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	if (GetRoomComp<USectionCeilingComponent>(aRoom))
	{
		// Indoor grapple
	}
	else
	{
		// Outdoor grapple
		const float height = aRoom.groundOffset + aRoom.ceilHeight + myHeight;
		const FVector location = FVector(aRoom.center.X, aRoom.center.Y, height);
		const FRotator rotation = FRotator(90.0f, 0.0f, 0.0f);
		const FTransform trans = FTransform(rotation, location);
		aGenerator->SpawnGeneratedActor(myGrappleClass, trans);
	}
}
