#include "LevelDataConfig.h"
#include "Project/Level/FaceMeshGeneration.h"

void ULevelDataConfig::Populate(ALevelData* aLevelData, const TMap<int32, FLevelDataFace>& someFaces, const TArray<int32> aPath, const FVector& anOffset)
{
	for (const auto& index : aPath)
		PopulateFace(aLevelData, someFaces[index], anOffset);
}

void ULevelDataConfig::PopulateFace(ALevelData* aLevelData, const FLevelDataFace& aFace, const FVector& anOffset)
{
	// Create ground and ceiling
	const float groundOffset = FMath::RandRange(myGroundMinOffset, myGroundMaxOffset);
	const float ceilHeight = FMath::RandRange(myCeilingMinHeight, myCeilingMaxHeight);
	if (myHasGround)
	{
		const FVector offset = anOffset + FVector(0, 0, groundOffset);
		CreateFaceMesh(aLevelData, aFace, offset, myGroundThickness, myGroundMaterial);
	}
	if (myHasCeil)
	{
		const FVector offset = anOffset + FVector(0, 0, groundOffset + ceilHeight);
		CreateFaceMesh(aLevelData, aFace, offset, myCeilingThickness, myCeilingMaterial);
	}

	// Find connections
	TArray<FVector2D> pathEdgeLocations;
	for (auto& edge : aFace.edges)
		if (edge.Value.hasWall)
			pathEdgeLocations.Add(edge.Value.location);

	auto isConnection = [&](const FVector2D aCurr, const FVector2D aNext)
	{
		for (const auto& edge : pathEdgeLocations)
		{
			const float dist = FMath::PointDistToSegmentSquared(
				FVector(edge.X, edge.Y, 0.0f),
				FVector(aCurr.X, aCurr.Y, 0.0f),
				FVector(aNext.X, aNext.Y, 0.0f));
			if (dist < SMALL_NUMBER)
				return true;
		}
		return false;
	};
	
	// Find wall vertices
	TArray<TArray<FVector2D>> walls;
	walls.Emplace();
	for (int32 i = 0; i < aFace.vertices.Num() - 1; i++)
	{
		auto& curr = aFace.vertices[i];
		auto& next = aFace.vertices[i + 1];
		if (isConnection(curr, next))
		{
			// Create new wall segment
			walls.Emplace();
			continue;
		}

		// Add wall vertex
		walls.Last().Add({curr.X, curr.Y});
	}
	// Add last vertex
	if (aFace.vertices.Num())
		walls.Last().Add(aFace.vertices.Last());

	// Create wall
	for (const auto& wall : walls)
		CreateWall(aLevelData, wall, groundOffset, ceilHeight);
	
}