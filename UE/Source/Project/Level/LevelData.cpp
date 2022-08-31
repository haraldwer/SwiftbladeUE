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

void ALevelData::BeginPlay()
{
	Super::BeginPlay();
	GenerateLevel();
}

void ALevelData::GenerateLevel()
{
	const auto config = GetConfig();
	CHECK_RETURN_LOG(!config, "No config");

	Clear();

	// Set seed
	if (mySeed)
		FMath::RandInit(mySeed);

	// Create a number of sections, one at a time
	// Each section is a point
	// Random number of vertices
	// Random distribution
	// Select edge when creating next room
	
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
	
	struct AllEdge
	{
		FVector2D start;
		FVector2D end;
	};
	
	TArray<FLevelDataRoom> rooms;
	TArray<AllEdge> allEdges;
	while(rooms.Num() < config->myNumRooms)
	{
		const float roomRadius = FMath::RandRange(config->myMinRoomRadius, config->myMaxRoomRadius);
		
		FVector2D srcEdgeStart;
		FVector2D srcEdgeEnd;
		FVector2D roomLoc;
		
		if (rooms.Num())
		{
			// Use prev room
			auto& prev = rooms.Last();

			// Select edge
			int32 edgeIndex = -1;
			if (config->mySnakePath)
			{
				// Select random edge within threshold
				for (int i = 0; i < 50; i++)
				{
					const int32 index = FMath::RandRange(0, prev.edges.Num() - 1);
					if (prev.edges[index].normal.Y < config->mySnakePathY)
						continue;
					edgeIndex = index;
					break;
				}
			}
			if (!config->mySnakePath || edgeIndex == -1)
			{
				// Select the straightest edge
				float normalY = 0.0f;
				for (int i = 0; i < prev.edges.Num(); i++)
				{
					if (prev.edges[i].normal.Y > normalY)
					{
						normalY = prev.edges[i].normal.Y;
						edgeIndex = i;
					}
				}
				if (edgeIndex == -1)
					edgeIndex = 0; 
			}

			auto& edge = prev.edges[edgeIndex];
			srcEdgeStart = prev.vertices[edge.startIndex];
			srcEdgeEnd = prev.vertices[edge.endIndex];
			edge.isPath = true;

			// TODO: Calculate location in sphere
			const float dist = FVector2D::Distance(srcEdgeStart, srcEdgeEnd) * 0.5f;
			const float d2 = dist * dist;
			const float r2 = roomRadius * roomRadius;
			const float l = FMath::Sqrt(r2 - d2);
			roomLoc = (srcEdgeStart + srcEdgeEnd) * 0.5f + edge.normal * l;
		}
		else
		{
			// Is start
			srcEdgeStart = FVector2d(roomRadius, 0.0f);
			srcEdgeEnd = FVector2d(-roomRadius, 0.0f);
			roomLoc = FVector2d(0.0f, 1.0f);
		}

		const FVector2D srcEdgeLoc = (srcEdgeStart + srcEdgeEnd) * 0.5f;
		
		auto& room = rooms.Emplace_GetRef();
		room.location = roomLoc;
		room.radius = roomRadius;
		room.vertices.Add(srcEdgeStart);
		room.vertices.Add(srcEdgeEnd);
		if (myDebugDrawNoise)
			DrawDebugSphere(GetWorld(), FVector(roomLoc.X, roomLoc.Y, 0.0f), 10.0f, 8, FColor(255, 0, 0), true);

		const FVector2D middle = (srcEdgeStart + srcEdgeEnd) * 0.5f;
		const FVector2D srcDir = (srcEdgeEnd - srcEdgeStart).GetSafeNormal();
		const FVector2D srcEdgeStartExt = middle - srcDir * 10000.0f;
		const FVector2D srcEdgeEndExt = middle + srcDir * 10000.0f;
		
		// Create vertices
		const int32 numVerts = FMath::RandRange(config->myMinNumVerts, config->myMaxNumVerts);
		while (room.vertices.Num() < numVerts)
		{
			// Check line intersect
			FVector2D intersect;
			FVector2D vert = FMath::RandPointInCircle(1.0f).GetSafeNormal() * roomRadius + roomLoc;
			if (lineIntersection(roomLoc, vert,
				srcEdgeStartExt, srcEdgeEndExt, intersect))
				continue;

			// Push vertices inwards, to prevent clipping
			for (const auto& edge : allEdges)
				if (lineIntersection(roomLoc, vert,
					edge.start, edge.end, intersect))
					vert = intersect;

			// Edge case for last room, flatten
			if (rooms.Num() == config->myNumRooms)
				vert.Y = FMath::Min(roomLoc.Y + 0.01f, vert.Y);
			
			room.vertices.Add(vert);
		}

		// Edge case for last room, make sure there is an exit
		if (rooms.Num() == config->myNumRooms)
			room.vertices.Add(roomLoc + FVector2D(0, 0.01f));

		// Sort clockwise
		room.vertices.Sort([&](const FVector2D& aFirst, const FVector2D& aSecond)
		{
			const FVector2D firstDiff = (aFirst - roomLoc).GetSafeNormal();
			const FVector2D secondDiff = (aSecond - roomLoc).GetSafeNormal();
			return FMath::Atan2(firstDiff.Y, firstDiff.X) < FMath::Atan2(secondDiff.Y, secondDiff.X);
		});
		
		// Create edges from vertices and calculate center
		for (int32 i = 0; i < room.vertices.Num(); i++)
		{
			const int32 startI = i;
			const int32 endI = (i + 1) % room.vertices.Num(); 
			const FVector2D start = room.vertices[startI];
			const FVector2D end = room.vertices[endI];
			const FVector2D dir = (end - start).GetSafeNormal();
			const FVector2D normal = { dir.Y, -dir.X }; 
			const FVector2D edgeLoc = (start + end) * 0.5f;
			const bool isPath = FVector2D::DistSquared(edgeLoc, srcEdgeLoc) < SMALL_NUMBER;
			
			const FLevelDataEdge edge = {
				startI, endI,
				normal,
				edgeLoc,
				isPath
			};
			
			room.edges.Add(edge);			 
			allEdges.Add({
				start, end
			});

			room.center += start;
		}
		room.center *= 1.0f / static_cast<float>(room.vertices.Num());
	}

	const FVector2D lastRoomLoc = rooms.Last().location;
	TArray<AActor*> levelEnds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelEndLocation::StaticClass(), levelEnds);
	for (AActor* actor : levelEnds)
		if (const auto levelEnd = Cast<ALevelEndLocation>(actor))
			levelEnd->SetActorLocation(FVector(lastRoomLoc.X, lastRoomLoc.Y, 0.0f));

	// Generate walls
	for (auto& room : rooms)
	{
		// Find connections
		TArray<FVector2D> pathEdgeLocations;
		for (auto& edge : room.edges)
			if (edge.isPath)
				pathEdgeLocations.Add(edge.location);

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
		room.walls.Emplace();
		for (int32 i = 0; i < room.vertices.Num(); i++)
		{
			auto& curr = room.vertices[i % room.vertices.Num()];
			auto& next = room.vertices[(i + 1) % room.vertices.Num()];
			const bool noWall = isConnection(curr, next);

			if (myDebugDrawNoise)
				DrawDebugLine(GetWorld(),
					FVector(curr.X, curr.Y, 0.0f),
					FVector(next.X, next.Y, 0.0f),
					noWall ? FColor(255, 0, 0) : FColor(0, 255, 0),
					true);

			auto& section = room.walls.Last(); 
			if (noWall)
			{
				if (section.verts.Num())
					room.walls.Emplace(); 
				continue;
			}

			if (!section.verts.Num())
				section.verts.Add(curr);
			section.verts.Add(next);
		}
	}

	config->Populate(this, rooms);
}

void ALevelData::Clear()
{
	// Destroy existing objects
	for (auto& type : myGeneratedObjects)
	{
		for (auto& obj : type.Value)
		{
			if (const auto comp = Cast<UActorComponent>(obj))
				comp->DestroyComponent();
			if (const auto actor = Cast<AActor>(obj))
				actor->Destroy();
		}
	}
	myGeneratedObjects.Reset();
}

AActor* ALevelData::SpawnGeneratedActor(const TSubclassOf<AActor>& anActorType, const FTransform& aTrans)
{
	AActor* actor = GetWorld()->SpawnActor(anActorType, &aTrans);
	if (actor)
		AddGeneratedObject(actor);
	return actor;
}

void ALevelData::AddGeneratedObject(UObject* anObject)
{
	CHECK_RETURN_LOG(!anObject, "Invalid object");
	myGeneratedObjects.FindOrAdd(anObject->GetClass()).Add(anObject);
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
