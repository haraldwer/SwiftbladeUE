#include "SectionCompPathSpline.h"

#include "Components/SplineComponent.h"
#include "Project/LevelGen/Section/SectionGenerator.h"
#include "Project/ProcGen/ProcSpline.h"

void USectionCompPathSpline::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection,const FProcRoom& aRoom)
{
	const FVector actorLoc = FVector(aRoom.location.X, aRoom.location.Y, aRoom.groundOffset); 
	AActor* actor = aGenerator->SpawnGeneratedActor(myClass, FTransform(actorLoc));
	CHECK_RETURN_LOG(!actor->Implements<UProcSplineMesh>(), "Does not implement interface");
	const auto spline = IProcSplineMesh::Execute_GetSpline(actor, 0);
	CHECK_RETURN_LOG(!spline, "No spline");
	spline->ClearSplinePoints(false);
	const float part = 1.0f / static_cast<float>(myPathPrecision + 1);
	for (int32 i = 0; i < myPathPrecision; i++)
	{
		const float progress = ((i * part) * (1.0f - myPathPadding * 2.0f)) + myPathPadding;				
		const float index = progress * aRoom.path.Num();
		const FVector2D pathLoc = GetBlendVert(aRoom.path, index);
		const FVector location = FVector(pathLoc.X, pathLoc.Y, aRoom.groundOffset) + GetOffset();
		spline->AddSplinePoint(location, ESplineCoordinateSpace::World, false);
		//spline->SetScaleAtSplinePoint(spline->GetNumberOfSplinePoints(), )
	}
	spline->UpdateSpline();
	IProcSplineMesh::Execute_GenerateMesh(actor);
}
