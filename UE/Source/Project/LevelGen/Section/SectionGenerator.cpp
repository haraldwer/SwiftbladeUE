#include "SectionGenerator.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "ProceduralMeshComponent.h"
#include "SectionDataConfig.h"
#include "SectionDataStructs.h"
#include "Components/SectionCompBase.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Utility/Math/LineIntersection.h"

ASectionGenerator::ASectionGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASectionGenerator::BeginPlay()
{
	Super::BeginPlay();

	Generate();
}

void ASectionGenerator::Generate()
{
	LOG("Generate");
	
	Clear();

	FlushPersistentDebugLines(GetWorld());
	
	// Set seed
	if (mySeed)
		FMath::RandInit(mySeed);

	const FVector levelLoc = GetActorLocation();
	FVector2D lastSectionEnd = FVector2D(levelLoc.X, levelLoc.Y);
	float lastSectionHeight = levelLoc.Z;
	for (int i = 0; i < myNumSections; i++)
	{
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
}

void ASectionGenerator::Clear()
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

AActor* ASectionGenerator::SpawnGeneratedActor(const TSubclassOf<AActor>& anActorType, const FTransform& aTrans)
{
	if (AActor* actor = GetWorld()->SpawnActor(anActorType, &aTrans))
	{
		AddGeneratedObject(actor);
		return actor;
	}
	return nullptr;
}

void ASectionGenerator::AddGeneratedObject(UObject* anObject)
{
	CHECK_RETURN_LOG(!anObject, "Invalid object");
	myGeneratedObjects.FindOrAdd(anObject->GetClass()).Add(anObject);
}

USectionDataConfig* ASectionGenerator::GetRandomConfig() const
{
	CHECK_RETURN_LOG(!myConfigs.Num(), "No configs", nullptr);
	const int32 index = FMath::RandRange(0, myConfigs.Num() - 1);
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
		
		float roomRadius = FMath::RandRange(aConfig.myMinRoomRadius, aConfig.myMaxRoomRadius);
		
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
					const int32 index = FMath::RandRange(0, prev.edges.Num() - 1);
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
		const int32 numVerts = FMath::RandRange(aConfig.myMinNumVerts, aConfig.myMaxNumVerts);
		while (room.vertices.Num() < numVerts && vertItr < numVerts * 10)
		{
			vertItr++;
			
			FVector2D intersect;
			const FVector2D vert = FMath::RandPointInCircle(1.0f).GetSafeNormal() * roomRadius + roomLoc;
			
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
		TArray<FProcEdge*> pathEdgeLocations;
		for (auto& edge : room.edges)
			if (edge.isPath)
				pathEdgeLocations.Add(&edge);

		auto isConnection = [&](const int32 aCurr, const int32 aNext)
		{
			for (const auto& edge : pathEdgeLocations)
			{
				if (edge->startIndex == aCurr && edge->endIndex == aNext)
					return true;
			}
			return false;
		};
	
		// Find wall vertices
		room.walls.Emplace();
		for (int32 i = 0; i < room.vertices.Num(); i++)
		{
			const int32 currI = i % room.vertices.Num();
			const int32 nextI = (i + 1) % room.vertices.Num();
			const bool noWall = isConnection(currI, nextI);

			auto& curr = room.vertices[currI];
			auto& next = room.vertices[nextI];
			if (myDebugDrawNoise)
				DrawDebugLine(GetWorld(),
					FVector(curr.X, curr.Y, 200.0f),
					FVector(next.X, next.Y, 200.0f),
					noWall ? FColor(255, 0, blue) : FColor(0, 255, blue),
					true);

			auto& wallSection = room.walls.Last(); 
			if (noWall)
			{
				if (wallSection.verts.Num())
					room.walls.Emplace(); 
				continue;
			}

			if (!wallSection.verts.Num())
				wallSection.verts.Add(curr);
			wallSection.verts.Add(next);
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
		const bool applyGroundOffset = i != 0 && FMath::RandRange(0.0f, 100.0f) < aConfig.myGroundOffsetChance;
		room.groundOffset = prevRoomHeight + FMath::RandRange(aConfig.myGroundMinOffset, aConfig.myGroundMaxOffset) * applyGroundOffset;
		room.ceilHeight = FMath::RandRange(aConfig.myCeilingMinHeight, aConfig.myCeilingMaxHeight);
		prevRoomHeight = room.groundOffset; 
	}
}

void ASectionGenerator::Populate(FProcSection& aSection, const USectionDataConfig& aConfig)
{	
	// Apply component chance for each room
	for (auto& comp : aConfig.myComponents)
		if (auto compPtr = comp.Get())
			for (const auto roomIndex : compPtr->PopulateSection(this, aSection))
				if (aSection.rooms.IsValidIndex(roomIndex))
					aSection.rooms[roomIndex].components.Add(compPtr);

	for (auto& room : aSection.rooms)
	{
		// Sort and filter room components
		auto comps = FilterSortRoomComponents(room);
		room.components = comps;

		// Then, populate
		for (const auto& comp : comps)
			if (const auto compPtr = Cast<USectionCompBase>(comp))
				compPtr->PopulateRoom(this, aSection, room);
	}
}

TArray<USectionCompBase*> ASectionGenerator::FilterSortRoomComponents(const FProcRoom& aRoom)
{
	const auto& comps = aRoom.components;
	CHECK_RETURN_LOG(!comps.Num(), "No components in room", {});

	// Sort components based on requirements
	// Any requirement means that the required component has to come before
	
	TArray<USectionCompBase*> sortedComps;
	for (const auto comp : comps)
		AddSortedComponent(comps, sortedComps, comp, 0);
	
	// Filter out blocked components
	const auto isBlocked = [&](const USectionCompBase& aComp)
	{
		const auto& blockingComps = aComp.GetBlockingComps();
		for (auto& blockingComp : blockingComps)
		{
			const auto find = sortedComps.FindByPredicate(
				[&blockingComp, &aComp] (const USectionCompBase* aCompare)
				{
					if (aCompare != &aComp)
					 	return aCompare->IsA(blockingComp);
					return false;
				});
			
			if (find && *find)
				return true;
		}
		return false; 
	};
	TArray<USectionCompBase*> filteredComps;
	for (auto& comp : sortedComps)
	{
		CHECK_CONTINUE_LOG(!comp, "Component null");
		if (!isBlocked(*comp))
			filteredComps.Add(comp);
	}
	
	return filteredComps;
}

void ASectionGenerator::AddSortedComponent(const TArray<USectionCompBase*>& aBase, TArray<USectionCompBase*>& aResult, USectionCompBase* aComp, const int32 aDepth)
{
	CHECK_RETURN_LOG(!aComp, "Component null");
	CHECK_RETURN_LOG(aDepth > 10, "Recursive component dependency found, exiting loop");
	CHECK_RETURN(aResult.Contains(aComp));
		
	// This component requires these components to be prioritized above it
	const auto& requiredComps = aComp->GetReqiredComps();
	for (auto& requiredComp : requiredComps)
	{	
		// Add required comp before current comp
		// We've only got the type, find the actual object
		const auto objFind = aBase.FindByPredicate(
			[&requiredComp](const USectionCompBase* aCompare)
			{ return aCompare->IsA(requiredComp); });
		CHECK_RETURN_LOG(!objFind || !(*objFind), "Could not find required component in config components");
		AddSortedComponent(aBase, aResult, *objFind, aDepth + 1);
	}

	// Add this comp
	aResult.Add(aComp);
}
