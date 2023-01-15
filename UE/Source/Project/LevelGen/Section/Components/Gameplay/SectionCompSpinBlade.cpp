#include "SectionCompSpinBlade.h"

#include "Components/SplineComponent.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Section/SectionGenerator.h"
#include "Project/ProcGen/ProcSpline.h"

void USectionCompSpinBlade::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	const FVector actorLoc = FVector(aRoom.location.X, aRoom.location.Y, aRoom.groundOffset); 
	AActor* actor = aGenerator->SpawnGeneratedActor(myBladeClass, FTransform(actorLoc));
	CHECK_RETURN_LOG(!actor, "No actor spawned");
	CHECK_RETURN_LOG(!actor->Implements<UProcSplineMesh>(), "Does not implement interface");
	const auto spline = IProcSplineMesh::Execute_GetSpline(actor, 0);
	CHECK_RETURN_LOG(!spline, "No spline");
	spline->ClearSplinePoints(false);
	
	switch (myLocation)
	{
	case ESpinBladeLocation::FLOOR:
		{
			FVector2D first;
			FVector2D second;
			if (!GetRandomCrossSection(aRoom, first, second))
				break;
			spline->AddSplinePoint(FVector(first.X, first.Y, aRoom.groundOffset + myFloorHeight), ESplineCoordinateSpace::World, false);
			spline->AddSplinePoint(FVector(second.X, second.Y, aRoom.groundOffset + myFloorHeight), ESplineCoordinateSpace::World, false);
			SetRoll(actor, 90.0f); 
			break;
		}
	case ESpinBladeLocation::WALLS:
		{	
			CHECK_BREAK_LOG(!aRoom.walls.Num(), "Room had no walls");			
			const int32 index = ULevelRand::RandRange(0, aRoom.walls.Num() - 1);
			const auto& wall = aRoom.walls[index];
			for (int32 vertI = 0; vertI < wall.verts.Num(); vertI++)
			{
				const auto& vert = wall.verts[vertI];
				const auto dir = (aRoom.center - vert).GetSafeNormal();
				const auto loc = vert + dir * myWallDist;
				const float maxHeight = FMath::Min(aRoom.ceilHeight - myWallCeilPadding, myWallMaxHeight);
				const float height = ULevelRand::FRandRange(myWallGroundPadding, maxHeight);
				spline->AddSplinePoint(FVector(loc.X, loc.Y, aRoom.groundOffset + height), ESplineCoordinateSpace::World, false);
			}

			SetRoll(actor, 180.0f); 
			break;
		}
	}

	spline->UpdateSpline();
	
	if (spline->GetNumberOfSplinePoints() < 2)
	{
		LOG("No spline points, destroying"); 
		actor->Destroy();
		return; 
	}
	
	for (int32 i = 0; i < spline->GetNumberOfSplinePoints(); i++)
	{
		auto tangent = spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		spline->SetTangentAtSplinePoint(i, tangent * 0.2f, ESplineCoordinateSpace::Local); 
	}
	
	IProcSplineMesh::Execute_GenerateMesh(actor); 
}
