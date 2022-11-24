#include "SectionCompCave.h"

#include "Components/SplineComponent.h"
#include "Project/LevelGen/Section/SectionGenerator.h"
#include "Project/ProcGen/ProcSpline.h"

TArray<int32> USectionCompCave::PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	TArray<int32> indices = Super::PopulateSection(aGenerator, aSection);
	indices.Sort();
	
	AActor* actor = nullptr;
	USplineComponent* spline = nullptr;
	int32 prevIndex = -1;
	for (const int32 index : indices)
	{
		if (prevIndex < 0 || index - 1 != prevIndex)
		{
			if (spline && actor)
			{
				spline->UpdateSpline();
				IProcSplineMesh::Execute_GenerateMesh(actor);
			}
			
			actor = aGenerator->SpawnGeneratedActor(myClass, FTransform::Identity);
			CHECK_RETURN_LOG(!actor->Implements<UProcSplineMesh>(), "Does not implement interface", indices);
			spline = IProcSplineMesh::Execute_GetSpline(actor, 0);
			CHECK_RETURN_LOG(!spline, "No spline", indices);
			spline->ClearSplinePoints(false);
		}

		CHECK_RETURN(!actor, indices);
		CHECK_RETURN(!spline, indices);

		const FProcRoom& room = aSection.rooms[index];
		for (auto& edge : room.edges)
		{
			CHECK_CONTINUE(!edge.isPath)
			CHECK_CONTINUE(edge.normal.Y >= 0);

			// Some fancy interp betwen prev and curr room
			const float groundOffset = room.groundOffset;
			const float height = room.ceilHeight;
			const float width = FVector2D::Distance(room.vertices[edge.startIndex], room.vertices[edge.endIndex]);
			spline->AddSplinePoint(
				FVector(edge.location.X, edge.location.Y, groundOffset + height * 0.5f),
				ESplineCoordinateSpace::World,
				false);
			spline->SetScaleAtSplinePoint(
				spline->GetNumberOfSplinePoints() - 1,
				FVector(100.0f, height * 1.4f, width) * 0.005f,
				false);
			spline->SetTangentAtSplinePoint(
				spline->GetNumberOfSplinePoints() - 1,
				-FVector(edge.normal.X, edge.normal.Y, 0.0f) * room.radius,
				ESplineCoordinateSpace::World,
				false);
		}
		
		prevIndex = index; 
	}

	if (spline && actor)
	{
		// Add last point
		auto& lastRoom = aSection.rooms[indices.Last()];
		const float groundOffset = lastRoom.groundOffset;
		const float height = lastRoom.ceilHeight;
		const float width = lastRoom.radius * 1.5f; 
		spline->AddSplinePoint(
			FVector(aSection.lastEdgeLoc.X, aSection.lastEdgeLoc.Y, groundOffset + height * 0.5f),
			ESplineCoordinateSpace::World,
			false);
		spline->SetScaleAtSplinePoint(
			spline->GetNumberOfSplinePoints() - 1,
			FVector(100.0f, height * 1.4f, width) * 0.005f,
			false);
		spline->SetTangentAtSplinePoint(
			spline->GetNumberOfSplinePoints() - 1,
			FVector(0.0f, 1.0f, 0.0f) * lastRoom.radius,
			ESplineCoordinateSpace::World,
			false);

		// Update and generate
		spline->UpdateSpline();
		IProcSplineMesh::Execute_GenerateMesh(actor);
	}
	
	return indices;
}
