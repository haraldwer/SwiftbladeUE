#include "LevelData.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "LevelEndLocation.h"
#include "ProceduralMeshComponent.h"
#include "Config/LevelDataConfig.h"
#include "Kismet/GameplayStatics.h"

ALevelData::ALevelData()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALevelData::GenerateLevel()
{
	const auto config = GetConfig();
	CHECK_RETURN_LOG(!config, "No config");

	// Destroy existing objects
	for (auto& type : myGeneratedObjects)
	{
		for (auto& obj : type.Value)
		{
			if (const auto comp = Cast<UActorComponent>(obj))
			{
				comp->UnregisterComponent();
				comp->DestroyComponent();
			}

			if (const auto actor = Cast<AActor>(obj))
				actor->Destroy();
		}
	}
	myGeneratedObjects.Reset();

	// Set seed
	FMath::RandInit(mySeed);
	
	// Faces from noise
	TMap<int32, FLevelDataFace> faces;
	ConstructFaces(faces);

	// Path from faces
	const TArray<int32> path = GeneratePath(faces);
	const FVector offset = AdjustConnectingFaces(path, faces);

	config->Populate(this, faces, path, offset);
}

AActor* ALevelData::SpawnGeneratedActor(const TSubclassOf<AActor> anActorType)
{
	AActor* actor = GetWorld()->SpawnActor(anActorType);
	if (actor)
		AddGeneratedObject(actor);
	return actor;
}

void ALevelData::AddGeneratedObject(UObject* anObject)
{
	CHECK_RETURN_LOG(!anObject, "Invalid object");
	myGeneratedObjects.FindOrAdd(anObject->GetClass()).Add(anObject);
}

TArray<FVector2D> ALevelData::GeneratePoints() const
{
	const auto config = GetConfig();
	CHECK_RETURN_LOG(!config, "No config", {});
	
	TArray<FVector2D> points;
	for (int32 i = 0; i < config->myCells; i++)
		points.Add(FMath::RandPointInCircle(config->myNoiseRadius));
	return points; 
}

void ALevelData::ConstructFaces(TMap<int32, FLevelDataFace>& someFaces) const
{
	// Is closest point
	auto isClosest = [](const TArray<FVector2D>& somePoints, const float aDist, const FVector2D& aPoint, const int32 aI, const int32 aJ)
	{
		for (int32 k = 0; k < somePoints.Num(); k++)
		{
			CHECK_CONTINUE(k == aI);
			CHECK_CONTINUE(k == aJ);
			if (FVector2D::DistSquared(somePoints[k], aPoint) < (aDist * aDist))
				return false;
		}
		return true;
	};
	
	// Create voronoi pattern
	const TArray<FVector2D> points = GeneratePoints();
	
	for (int32 i = 0; i < points.Num(); i++)
	{
		auto& face = someFaces.FindOrAdd(i);
		face.location = points[i];
		
		if (myDebugDrawNoise)
		{
			DrawDebugSphere(GetWorld(), FVector(face.location.X, face.location.Y, 0.0f), 10.0f, 8, FColor(255, 0, 0), true);
		}
		
		// Loop though every other point
		for (int32 j = 0; j < points.Num(); j++)
		{
			CHECK_CONTINUE(i == j);

			// Face already added
			CHECK_CONTINUE(face.edges.Contains(j));
			
			// Find middle
			const FVector2D middle = (face.location + points[j]) * 0.5f;
			const FVector2D diff = points[j] - face.location;
			const float dist = diff.Length() * 0.5f;
			
			// If another point is closer to the middle
			CHECK_CONTINUE(!isClosest(points, dist, middle, i, j));
			// Discard

			// i and j are facing each other
			// Edge is middle
			const FVector2d normal = diff.GetSafeNormal();
			face.edges.FindOrAdd(j) = { normal, middle };
			someFaces.FindOrAdd(j).edges.FindOrAdd(i) = { -normal, middle };
			
			FVector2D dir = FVector2D(-normal.Y, normal.X);
			const FVector2D start = middle + dir * 30.0f;
			const FVector2D end = middle - dir * 30.0f;
			if (myDebugDrawNoise)
			{
				DrawDebugLine(GetWorld(), FVector(start.X, start.Y, 0.0f), FVector(end.X, end.Y, 0.0f), FColor(255, 255, 255), true);
				DrawDebugSphere(GetWorld(), FVector(middle.X, middle.Y, 0.0f), 10.0f, 8, FColor(0, 255, 0), true);
			}
		}
	}

	GenerateVertices(someFaces);
}

void ALevelData::GenerateVertices(TMap<int32, FLevelDataFace>& someFaces) const
{
	// Math functions for checking intersections
	constexpr auto checkIntersect = [](
		float& aResultDist, FVector2D& aResultPoint,
		const FVector2D& aFirstLoc, const FVector2D& aFirstDir,
		const FVector2D& aSecondLoc, const FVector2D& aSecondDir)
	{
		constexpr auto lineIntersection = [](
			const FVector2D& aSegmentStartA, const FVector2D& aSegmentEndA,
			const FVector2D& aSegmentStartB, const FVector2D& aSegmentEndB,
			FVector2D& anIntersect)
		{
			const FVector2D vectorA = aSegmentEndA - aSegmentStartA;
			const FVector2D vectorB = aSegmentEndB - aSegmentStartB;

			const FVector::FReal s = (-vectorA.Y * (aSegmentStartA.X - aSegmentStartB.X) + vectorA.X *
				(aSegmentStartA.Y - aSegmentStartB.Y)) / (-vectorB.X * vectorA.Y + vectorA.X * vectorB.Y);
			const FVector::FReal t = (vectorB.X * (aSegmentStartA.Y - aSegmentStartB.Y) - vectorB.Y *
				(aSegmentStartA.X - aSegmentStartB.X)) / (-vectorB.X * vectorA.Y + vectorA.X * vectorB.Y);

			const bool bIntersects = (s >= 0 && s <= 1 && t >= 0 && t <= 1);

			if (bIntersects)
			{
				anIntersect.X = aSegmentStartA.X + (t * vectorA.X);
				anIntersect.Y = aSegmentStartA.Y + (t * vectorA.Y);
			}

			return bIntersects;
		};
		
		constexpr float checkDist = 1000.0f; 
		FVector2D intersect;
		if (lineIntersection(
			aFirstLoc,
			aFirstLoc - aFirstDir * checkDist,
			aSecondLoc - aSecondDir * checkDist,
			aSecondLoc + aSecondDir * checkDist,
			intersect))
		{
			const float dist = FVector2D::DistSquared(intersect, aFirstLoc);
			if (dist < aResultDist || aResultDist < 0)
			{
				aResultDist = dist;
				aResultPoint = intersect;
			}
		}
		else if (aResultDist < 0)
		{
			aResultDist = checkDist * checkDist;
			aResultPoint = aFirstLoc - aFirstDir * checkDist; 
		}
	};
	
	// Generate mesh
    for (auto& face : someFaces)
    {
        auto& verts = face.Value.vertices;   
        for (const auto& edge : face.Value.edges)
        {
            // Every neighbor has a face
            // Every face has two intersection points
            // One on each side

            const FVector2D point = edge.Value.location;
            const FVector2D dir = FVector2D(-edge.Value.normal.Y, edge.Value.normal.X);
            
            float leftDist = -1.0f;
            float rightDist = -1.0f;
            FVector2D leftPoint = point;
            FVector2D rightPoint = point;

        	// Loop through your neighbors          
        	for (const auto& neighbor : face.Value.edges)
        	{
        		CHECK_CONTINUE(neighbor.Key == edge.Key);

        		const FVector2D secondPoint = neighbor.Value.location;
        		const FVector2D secondDir = FVector2D(-neighbor.Value.normal.Y, neighbor.Value.normal.X);
        		checkIntersect(leftDist, leftPoint, point, dir, secondPoint, secondDir);
        		checkIntersect(rightDist, rightPoint, point, -dir, secondPoint, secondDir);
        	}
        	
            // Loop through neighbors neighbors
        	if (const auto faceFind = someFaces.Find(edge.Key))
        	{
        		for (const auto& secondNeighbor : faceFind->edges)
        		{
        			CHECK_CONTINUE(secondNeighbor.Key == face.Key);

        			const FVector2D secondPoint = secondNeighbor.Value.location;
        			const FVector2D secondDir = FVector2D(-secondNeighbor.Value.normal.Y, secondNeighbor.Value.normal.X);
        			checkIntersect(leftDist, leftPoint, point, dir, secondPoint, secondDir);
        			checkIntersect(rightDist, rightPoint, point, -dir, secondPoint, secondDir);
        		}
        	}
        	
            // Only add if not duplicate
            if (verts.Num() <= 0 || FVector2D::DistSquared(verts.Last(), rightPoint) > SMALL_NUMBER)
	            verts.Add(rightPoint);
            if (verts.Num() <= 0 || FVector2D::DistSquared(verts.Last(), leftPoint) > SMALL_NUMBER)
	            verts.Add(leftPoint);
        }

        if (verts.Num())
        {
            // Sort vertices based on direction
            verts.Sort([&](const FVector2D& aFirst, const FVector2D& aSecond)
            {
                const FVector2D firstDiff = aFirst - face.Value.location;
                const float firstAtan = FMath::Atan2(firstDiff.Y, firstDiff.X);
                const FVector2D secondDiff = aSecond - face.Value.location;
                const float secondAtan = FMath::Atan2(secondDiff.Y, secondDiff.X); 
                return firstAtan < secondAtan;
            });

        	if (myDebugDrawNoise)
        	{
        		// Debug draw
        		const FColor c = FColor(FMath::RandRange(0, 255), FMath::RandRange(0, 255), FMath::RandRange(0, 255));
        		for (int32 i = 0; i < verts.Num(); i++)
        		{
        			const int32 curr = i;
        			const int32 next = (i + 1) % verts.Num();

        			DrawDebugLine(GetWorld(),
						FVector(verts[curr].X, verts[curr].Y, 0.0f),
						FVector(verts[next].X, verts[next].Y, 0.0f),
						c, true);
        		}
        	}
        }
    }
}

TArray<int32> ALevelData::GeneratePath(TMap<int32, FLevelDataFace>& someFaces) const
{
	const auto config = GetConfig();
	CHECK_RETURN_LOG(!config, "No config", {});
	
	// Find first face
	int32 currFace = -1;
	int32 prevEdge = -1;
	float currEdgeY = 0.0f;
	for (auto& face : someFaces)
	{
		for (const auto& edge : face.Value.edges)
		{
			if (currFace > -1 && edge.Value.location.Y >= currEdgeY)
				continue;
			currFace = face.Key;
			prevEdge = edge.Key;
			currEdgeY = edge.Value.location.Y;
		}
	}
	
	// Generate path
	TArray<int32> path;
	while (currFace >= 0)
	{
		const auto face = someFaces.Find(currFace);
		CHECK_RETURN_LOG(!face, "Could not find face", {});
		path.Add(currFace);

		// Set previous edge as path
		if (const auto prevEdgePtr = face->edges.Find(prevEdge))
			prevEdgePtr->isPath = true;

		prevEdge = currFace; 
		currFace = -1; 
		for (auto& edge : face->edges)
		{
			const float dot = FVector2D::DotProduct(FVector2D(0.0f, 1.0f), edge.Value.normal);
			CHECK_CONTINUE(dot < 0.0f);
			
			// Set new edge as path
			currFace = edge.Key;
			edge.Value.isPath = true;
			break;
		}
	}
	return path;
}

FVector ALevelData::AdjustConnectingFaces(const TArray<int32>& aPath, TMap<int32, FLevelDataFace>& someFaces) const
{
	const auto config = GetConfig();
	CHECK_RETURN_LOG(!config, "No config", FVector::ZeroVector);
	CHECK_RETURN_LOG(!aPath.Num(), "No path", FVector::ZeroVector);
	
	// Find first edge
	FLevelDataFace& firstFace = someFaces[aPath[0]];
	int32 firstEdge = -1;
	for (const auto& edge : firstFace.edges)
		if (firstEdge == -1 || edge.Value.location.Y < firstFace.edges[firstEdge].location.Y)
			firstEdge = edge.Key;
	
	// Find last edge
	FLevelDataFace& lastFace = someFaces[aPath.Last()];
	int32 lastEdge = -1;
	for (const auto& edge : lastFace.edges)
		if (lastEdge == -1 || edge.Value.location.Y > lastFace.edges[lastEdge].location.Y)
			lastEdge = edge.Key;
	
	CHECK_RETURN_LOG(firstEdge == -1, "No first edge", FVector::ZeroVector);
	CHECK_RETURN_LOG(lastEdge == -1, "No last edge", FVector::ZeroVector);
	
	// Adjust vertices
	const float startY = firstFace.edges[firstEdge].location.Y - config->myConnectionPadding;
	const float endY = lastFace.edges[lastEdge].location.Y + config->myConnectionPadding;
	for (auto& index : aPath)
		for (auto& vert : someFaces[index].vertices)
			vert.Y = FMath::Min(endY, FMath::Max(startY, vert.Y));

	// Adjust edge connections
	for (const int32 index : aPath) 
		for (auto& edge : someFaces[index].edges)
			edge.Value.hasWall = !(edge.Value.isPath || aPath.Contains(edge.Key));
	
	const FVector startOffset = -FVector(firstFace.location.X, startY, 0.0f); 
	
	TArray<AActor*> levelEnds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelEndLocation::StaticClass(), levelEnds);
	for (AActor* actor : levelEnds)
		if (const auto levelEnd = Cast<ALevelEndLocation>(actor))
			levelEnd->SetActorLocation(startOffset + FVector(lastFace.location.X, endY, 0.0f));

	return startOffset; 
}

ULevelDataConfig* ALevelData::GetConfig() const
{
	const auto configPtr = myConfigs.Find(myType);
	CHECK_RETURN_LOG(!configPtr, "No config for face type", nullptr);
	const auto configType = *configPtr;
	const auto config = configType.GetDefaultObject();
	CHECK_RETURN_LOG(!config, "No config for face type", nullptr);
	return config;
}