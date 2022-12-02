#include "SectionGenerator.h"

#include "DrawDebugHelpers.h"
#include "SectionDataConfig.h"
#include "SectionDataStructs.h"
#include "Components/SectionCompBase.h"
#include "Components/SectionCompGroup.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Level/LevelEnd.h"
#include "Project/Utility/Math/LineIntersection.h"

void ASectionGenerator::Generate()
{
	Super::Generate();
	
	const FVector levelLoc = GetActorLocation();
	FVector2D lastSectionEnd = FVector2D(levelLoc.X, levelLoc.Y);
	float lastSectionHeight = levelLoc.Z;
	for (int i = 0; i < myNumSections; i++)
	{
		ULevelRand::Reset(i);
		
		const USectionDataConfig* const config = GetRandomConfig();
		CHECK_RETURN_LOG(!config, "No config");
		
		FProcSection section;
		GenerateSection(section, *config, lastSectionEnd);
		GenerateLastEdge(section, *config);
		GenerateWalls(section, *config);
		GenerateRoomPath(section);
		GenerateGroundCeil(section, *config, lastSectionHeight);
		Populate(section, *config);
		
		lastSectionEnd = section.lastEdgeLoc;
		lastSectionHeight = section.rooms.Last().groundOffset;
	}

	const FTransform trans = FTransform(FVector(lastSectionEnd.X, lastSectionEnd.Y, lastSectionHeight));
	if (const auto levelEnd = Cast<ALevelEnd>(SpawnGeneratedActor(myLevelEndClass, trans)))
	{
		levelEnd->OnEndLocationSet(this);
		myLevelEnd = levelEnd;
	}
	else LOG("Failed to spawn level end");
}

USectionDataConfig* ASectionGenerator::GetRandomConfig() const
{
	CHECK_RETURN_LOG(!myConfigs.Num(), "No configs", nullptr);
	const int32 index = ULevelRand::RandRange(0, myConfigs.Num() - 1);
	const auto configClass = myConfigs[index];
	const auto config = configClass.GetDefaultObject();
	CHECK_RETURN_LOG(!config, "No config for face type", nullptr);
	return config;
}

void ASectionGenerator::GenerateSection(FProcSection& aSection, const USectionDataConfig& aConfig, const FVector2D& aStartLocation) const
{
	// Create a number of sections, one at a time
	// Each section is a point
	// Random number of vertices
	// Random distribution
	// Select edge when creating next room
	
	auto checkDist = [](const TArray<FVector2D> anArr, const FVector2D& aVec, const float aMinDist)
	{
		for (const auto& otherVert : anArr)
			if (FVector2D::DistSquared(aVec, otherVert) < aMinDist * aMinDist)
				return true;
		return false; 
	};
	
	struct AllEdge
	{
		FVector2D start;
		FVector2D end;
	};

	TArray<AllEdge> allEdges;
	int32 roomItr = 0;
	while(aSection.rooms.Num() < aConfig.myNumRooms && roomItr < aConfig.myNumRooms * 3)
	{
		roomItr++;
		
		float roomRadius = ULevelRand::FRandRange(aConfig.myMinRoomRadius, aConfig.myMaxRoomRadius);
		
		FVector2D srcEdgeStart;
		FVector2D srcEdgeEnd;
		FVector2D roomLoc;

		const bool isFirstRoom = aSection.rooms.Num() == 0;
		const bool isLastRoom = aSection.rooms.Num() == aConfig.myNumRooms - 1;
		
		if (!isFirstRoom)
		{
			// Use prev room
			auto& prev = aSection.rooms.Last();

			// Select edge
			int32 edgeIndex = -1;
			if (aConfig.mySnakePath)
			{
				// Select random edge within threshold
				for (int i = 0; i < 50; i++)
				{
					const int32 index = ULevelRand::RandRange(0, prev.edges.Num() - 1);
					if (prev.edges[index].normal.Y < aConfig.mySnakePathY)
						continue;
					edgeIndex = index;
					break;
				}
			}
			if (!aConfig.mySnakePath || edgeIndex == -1)
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

			// Calculate distance
			const float dist = FVector2D::Distance(srcEdgeStart, srcEdgeEnd) * 0.5f;
			
			// Adjust room radius
			roomRadius = FMath::Max(dist + 1.0f, roomRadius);  

			// Calculate room location
			const float d2 = dist * dist;
			const float r2 = roomRadius * roomRadius;
			const float l = FMath::Sqrt(r2 - d2);
			roomLoc = (srcEdgeStart + srcEdgeEnd) * 0.5f + edge.normal * l;
		}
		else
		{
			// Is start

			float sin = 0.0f;
			float cos = 0.0f;
			float angle = FMath::DegreesToRadians(45);
			FMath::SinCos(&sin, &cos, angle);
			const FVector2D loc = FVector2D(cos, -sin) * roomRadius;
			
			roomLoc = aStartLocation + FVector2D(0.0f, -loc.Y);
			srcEdgeStart = roomLoc + loc;
			srcEdgeEnd = roomLoc + loc * FVector2D(-1.0f, 1.0f);
		}

		const FVector2D srcEdgeLoc = (srcEdgeStart + srcEdgeEnd) * 0.5f;
		
		auto& room = aSection.rooms.Emplace_GetRef();
		room.index = aSection.rooms.Num() - 1; 
		room.location = roomLoc;
		room.radius = roomRadius;
		room.vertices.Add(srcEdgeStart);
		room.vertices.Add(srcEdgeEnd);
		if (myDebugDrawNoise)
			DrawDebugSphere(GetWorld(),
				FVector(roomLoc.X, roomLoc.Y, 200.0f),
				10.0f, 8,
				FColor(255, 0, 0),
				true);

		const FVector2D middle = (srcEdgeStart + srcEdgeEnd) * 0.5f;
		const FVector2D srcDir = (srcEdgeEnd - srcEdgeStart).GetSafeNormal();
		const FVector2D srcEdgeStartExt = middle - srcDir * 10000.0f;
		const FVector2D srcEdgeEndExt = middle + srcDir * 10000.0f;

		if (isLastRoom)
		{
			float sin = 0.0f;
			float cos = 0.0f;
			float angle = FMath::DegreesToRadians(45);
			FMath::SinCos(&sin, &cos, angle);

			FVector2D loc = FVector2D(cos, sin) * roomRadius;

			// Adjust location for srcEdge
			const FVector2D worldLoc = roomLoc + loc; 
			if (worldLoc.Y < srcEdgeStart.Y || worldLoc.Y < srcEdgeEnd.Y)
			{
				// Select closest edge
				const FVector2D newLoc = srcEdgeStart.Y > srcEdgeEnd.Y ? srcEdgeStart : srcEdgeEnd;

				// Convert to room space
				loc = newLoc - roomLoc;

				// TODO: Risk of tight opening here

				if (myDebugDrawNoise)
					DrawDebugSphere(GetWorld(),
						FVector(newLoc.X, newLoc.Y, 200.0f),
						50.0f, 8,
						FColor(255, 255, 255),
						true);
				
				LOG("Adjusted for src");
			}
			
			room.vertices.Add(roomLoc + loc);
			room.vertices.Add(roomLoc + loc * FVector2D(-1.0f, 1.0f));
			
			allEdges.Add({
				roomLoc + FVector2D(10000.0f, loc.Y),
				roomLoc + FVector2D(-10000.0f, loc.Y)
			});
		}
		
		// Create vertices
		int32 vertItr = 0;
		const int32 numVerts = ULevelRand::RandRange(aConfig.myMinNumVerts, aConfig.myMaxNumVerts);
		while (room.vertices.Num() < numVerts && vertItr < numVerts * 10)
		{
			vertItr++;
			
			FVector2D intersect;
			const FVector dir = ULevelRand::RandVec(); 
			const FVector2D vert = FVector2D(dir.X, dir.Y).GetSafeNormal() * roomRadius + roomLoc;
			
			// Check line intersect
			if (LineIntersect(roomLoc, vert,
				srcEdgeStartExt, srcEdgeEndExt, intersect))
					continue;

			bool discard = false;
			for (const auto& edge : allEdges)
			{
				if (LineIntersect(roomLoc, vert,
				   edge.start, edge.end, intersect))
				{
					discard = true;
					break;
				}
			}
			if (discard)
				continue;
			
			// Check distance
			if (checkDist(room.vertices, vert, aConfig.myMinVertDist))
				continue;
			
			room.vertices.Add(vert);
		}

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

			// Not a valid edge
			if (FVector2D::DistSquared(start, end) < SMALL_NUMBER)
			{
				LOG("Small edge detected");
				continue;
			}
			
			const FVector2D dir = (end - start).GetSafeNormal();
			const FVector2D normal = { dir.Y, -dir.X }; 
			const FVector2D edgeLoc = (start + end) * 0.5f;
			const bool isPath = FVector2D::DistSquared(edgeLoc, srcEdgeLoc) < SMALL_NUMBER;
			
			const FProcEdge edge = {
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
}

void ASectionGenerator::GenerateLastEdge(FProcSection& aSection, const USectionDataConfig& aConfig) const
{
	FProcEdge* lastEdge = nullptr;
	for (auto& edge : aSection.rooms.Last().edges)
		if (!lastEdge || edge.location.Y > lastEdge->location.Y)
			lastEdge = &edge;
	if (lastEdge)
		lastEdge->isPath = true;
	aSection.lastEdgeLoc = lastEdge ?
		lastEdge->location : aSection.rooms.Last().location;
}

void ASectionGenerator::GenerateWalls(FProcSection& aSection, const USectionDataConfig& aConfig) const
{
	// Generate walls
	for (auto& room : aSection.rooms)
	{
		const int32 blue = FMath::RandRange(0, 255);
		
		// Find connections
		TArray<FProcEdge*> roomPathEdges;
		for (auto& edge : room.edges)
			if (edge.isPath)
				roomPathEdges.Add(&edge);

		const auto isPath = [&](const int32 aCurr, const int32 aNext)
		{
			for (const auto& edge : roomPathEdges)
			{
				if ((edge->startIndex == aCurr && edge->endIndex == aNext) ||
					(edge->startIndex == aNext && edge->endIndex == aCurr)) 
					return true;
			}
			return false;
		};
	
		// Find wall vertices
		FProcWall* wall = nullptr;
		const int32 startIndex = roomPathEdges.Num() ? roomPathEdges.Last()->endIndex : 0;
		for (int32 i = 0; i < room.vertices.Num(); i++)
		{
			const int32 currI = (i + startIndex) % room.vertices.Num();
			const int32 nextI = (i + startIndex + 1) % room.vertices.Num();
			const bool addWall = !isPath(currI, nextI);
			
			const auto& curr = room.vertices[currI];
			const auto& next = room.vertices[nextI];
			if (myDebugDrawNoise)
				DrawDebugLine(GetWorld(),
					FVector(curr.X, curr.Y, 200.0f),
					FVector(next.X, next.Y, 200.0f),
					!addWall ? FColor(255, 0, blue) : FColor(0, 255, blue),
					true);


			// Should there be a wall here?
			if (addWall)
			{
				// Yes!
				if (!wall)
				{
					wall = &room.walls.Emplace_GetRef();
					wall->verts.Add(curr); 
				}
				wall->verts.Add(next);
			}
			else
			{
				// No!
				wall = nullptr;
			}
		}
	}
}

void ASectionGenerator::GenerateRoomPath(FProcSection& aSection) const
{
	for (auto& room : aSection.rooms)
	{
		for (const auto& it : room.edges)
			if (it.isPath)
				room.path.Add(it.location);

		const int32 centerIndex = room.path.Num() ? 1 : 0;
		room.path.Insert(room.center, centerIndex);
	}
}

void ASectionGenerator::GenerateGroundCeil(FProcSection& aSection, const USectionDataConfig& aConfig, const float aStartHeight) const
{
	// Set room ground and ceil height 
	float prevRoomHeight = aStartHeight;
	for (int i = 0; i < aSection.rooms.Num(); i++)
	{
		auto& room = aSection.rooms[i];
		const bool applyGroundOffset = i != 0 && ULevelRand::FRandRange(0.0f, 100.0f) < aConfig.myGroundOffsetChance;
		room.groundOffset = prevRoomHeight + ULevelRand::FRandRange(aConfig.myGroundMinOffset, aConfig.myGroundMaxOffset) * applyGroundOffset;
		room.ceilHeight = ULevelRand::FRandRange(aConfig.myCeilingMinHeight, aConfig.myCeilingMaxHeight);
		prevRoomHeight = room.groundOffset; 
	}
}

void ASectionGenerator::Populate(FProcSection& aSection, const USectionDataConfig& aConfig)
{	
	// Apply component chance for each room
	for (auto& comp : aConfig.myComponents)
	{
		const auto compPtr = comp.Get();
		CHECK_CONTINUE(!compPtr)
		for (const auto roomIndex : compPtr->PopulateSection(this, aSection))
		{
			CHECK_CONTINUE(!aSection.rooms.IsValidIndex(roomIndex));
			auto& entry = aSection.rooms[roomIndex].components.Emplace_GetRef();
			entry.myPtr = compPtr;
			entry.myRequiredComps = compPtr->GetReqiredComps();
			entry.myBlockingComps = compPtr->GetBlockingComps();
		}
	}

	// Some rooms have sub-components, add these before sorting and filtering 
	for (auto& room : aSection.rooms)
		for (int32 i = 0; i < room.components.Num(); i++)
			if (const auto groupComp = Cast<USectionCompGroup>(room.components[i].myPtr))
				room.components.Append(groupComp->GetSubComponents(this, aSection, room));

	for (auto& room : aSection.rooms)
	{
		// Sort and filter room components
		auto comps = GetComponents(room.components);
		room.components = comps;

		// Then, populate
		for (const auto& comp : comps)
			if (comp.myPtr)
				comp.myPtr->PopulateRoom(this, aSection, room);
	}
}
