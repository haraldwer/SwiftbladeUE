 #include "SectionCompProp.h"

#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Project/Gameplay/Obstacle.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Props/PropConfig.h"
#include "Project/LevelGen/Props/PropGroup.h"
#include "Project/LevelGen/Section/SectionGenerator.h"
#include "Project/Utility/Math/LineIntersection.h"

void USectionCompProp::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	CHECK_RETURN_LOG(!aGenerator, "Generator nullptr");
	CHECK_RETURN_LOG(!myConfigs.Num(), "No configs in prop comp");

	// Select random config
	const int32 confI = ULevelRand::RandRange(0, myConfigs.Num() - 1);
	const auto configType = myConfigs[confI].Get();
	CHECK_RETURN_LOG(!configType, "ConfigType nullptr");
	const auto config = configType->GetDefaultObject<UPropConfig>(); 
	CHECK_RETURN_LOG(!config, "Config nullptr");

	// Fill rate should consider room area
	// TODO: Use vertices to calculate
	const float roomArea = aRoom.radius;
	const float countFloat = config->myFillRate * roomArea * 0.001f;
	const int32 count = FMath::RoundToInt32(countFloat); 
	for (int groupIndex = 0; groupIndex < count; groupIndex++)
		CreateGroup(*aGenerator, aRoom, *config);
}

void USectionCompProp::CreateGroup(ASectionGenerator& aGenerator, const FProcRoom& aRoom, UPropConfig& aConfig) const
{
	const FPropEntry* selected = SelectPropGroup(aConfig);

	CHECK_RETURN_LOG(!selected, "No selected prop");
	const auto group = selected->myGroup.Get();
	CHECK_RETURN_LOG(!group, "Group type nullptr");
	const auto defaultObject = group->GetDefaultObject<APropGroup>();
	CHECK_RETURN_LOG(!defaultObject, "Default object nullptr");

	// Try multiple times 
	for (int placementIndex = 0; placementIndex < 5; placementIndex++)
	{
		FVector2D loc = GetPlacement(aRoom, *defaultObject);
		FVector2D dir = (loc - aRoom.center).GetSafeNormal();

		// Adjust for walls and stuff
		AdjustPlacement(aGenerator, aRoom, *defaultObject, loc, dir);
		
		// Create transform
		const float yaw = FMath::RadiansToDegrees(FMath::Atan2(dir.Y, dir.X));
		const float yawRot = defaultObject->GetYawRot();
		const float rotOffset = ULevelRand::FRandRange(-yawRot * 0.5f, yawRot * 0.5f);
		const float height = aRoom.groundOffset + aRoom.ceilHeight * defaultObject->GetHeightPart();
		const FTransform trans = FTransform(FRotator(0.0f, yaw + rotOffset + 90.0f, 0.0f), FVector(loc.X, loc.Y, height));

		// Avoid overlaps with existing stuff
		if (FindOverlaps(aGenerator, *defaultObject, trans))
			continue;
		
		// Valid placement, spawn group and generate!
		if (const auto actor = Cast<APropGroup>(aGenerator.SpawnGeneratedActor(group, trans)))
			actor->Generate(&aGenerator);
			
		return;
	}
}

FPropEntry* USectionCompProp::SelectPropGroup(UPropConfig& aConfig)
{
	// Use a weighted approach for selecting group
	float totalWeight = 0.0f;
	for (const auto& entry : aConfig.myEntries)
		totalWeight += entry.mySpawnWeight;
		
	// Select group
	float weight = ULevelRand::FRandRange(0.0f, totalWeight);
	for (auto& prop : aConfig.myEntries)
	{
		weight -= prop.mySpawnWeight;
		if (weight <= 0.0f)
		{
			return &prop;
		}
	}
	return nullptr;
}

FVector2D USectionCompProp::GetPlacement(const FProcRoom& aRoom, const APropGroup& aGroupObject)
{
	const EPropPlacementMode placementMode = aGroupObject.GetPlacementMode();
	switch (placementMode)
	{
	case EPropPlacementMode::AGAINST_WALL:
	case EPropPlacementMode::CORNER:
		{
			if (aRoom.walls.Num())
			{
				// Random wall
				const FProcWall* wallPtr = nullptr;
				const int32 numWalls = aRoom.walls.Num();
				const int32 index = ULevelRand::RandRange(0, numWalls - 1);
				for (int32 iOff = 0; iOff < numWalls; iOff++)
				{
					const int32 newI = (index + iOff) % numWalls;
					auto& wall = aRoom.walls[newI];
					if (wall.verts.Num())
					{
						wallPtr = &wall;
						break;
					}
				}
		
				if (wallPtr)
				{
					// Random vert
					float vertI = ULevelRand::FRandRange(0.1f, wallPtr->verts.Num() - 0.1f);
					if (placementMode == EPropPlacementMode::CORNER)
						vertI = FMath::RoundToFloat(vertI);
					const FVector2D loc = GetBlendVert(wallPtr->verts, vertI);
					const FVector2D diff = loc - aRoom.center;
					const float dist = diff.Length();
					const FVector2D dir = diff / dist;
					const float min = FMath::Min(dist, aGroupObject.GetMinWallDist());
					const float max = FMath::Min(dist, aGroupObject.GetMaxWallDist());
					return loc - dir * ULevelRand::FRandRange(min, max);
				}
			}			
			break;
		}
	}

	// Fallthrough case
	{
		// Get random location in room
		const float min = FMath::Min(aRoom.radius, aGroupObject.GetMinWallDist());
		const float max = FMath::Min(aRoom.radius, aGroupObject.GetMaxWallDist());
		const FVector randVec = ULevelRand::RandVec();
		const float wallDist = ULevelRand::FRandRange(min, max);
		return aRoom.location +
			FVector2D(randVec.X, randVec.Y).GetSafeNormal() * (aRoom.radius - wallDist);
	}
}

void USectionCompProp::AdjustPlacement(ASectionGenerator& aGenerator, const FProcRoom& aRoom, const APropGroup& aGroupObject, FVector2D& aPlacement, FVector2D& aNormal)
{
	const FVector orgLoc = FVector(aPlacement.X, aPlacement.Y, aRoom.groundOffset + 100.0f);

	// Avoid corners
	const float cornerDist = aGroupObject.GetCornerDist();
	if (cornerDist > SMALL_NUMBER)
	{
		for (auto& corner : aRoom.vertices)
		{
			const FVector2D diff = aPlacement - corner;
			const float dist = diff.Size();
			if (dist < cornerDist)
				aPlacement = corner + diff.GetSafeNormal() * cornerDist;
		}
	}
	
	// Move inwards to avoid intersects
	// Repeat to avoid moving into another close wall 
	for (int j = 0; j < 2; j++)
	{
		for (auto& edge : aRoom.edges)
		{
			const FVector2D vert = aRoom.vertices[edge.startIndex];
			const FVector2D nextVert = aRoom.vertices[edge.endIndex];
			
			const FVector2D direction = (aPlacement - aRoom.center).GetSafeNormal();
			const float wallOffset = aGroupObject.GetWallOffset();
			const FVector2D edgeLoc = aPlacement + direction * wallOffset;
		
			static FVector2D result;
			if (LineIntersect(vert, nextVert, aRoom.center, edgeLoc, result))
			{
				aPlacement = result - direction * wallOffset;
				if (!edge.isPath && aGroupObject.GetPlacementMode() != EPropPlacementMode::CORNER)
					aNormal = edge.normal; 
			}
		}
	}

	const FVector newLoc = FVector(aPlacement.X, aPlacement.Y, aRoom.groundOffset + 100.0f);
	DrawDebugLine(aGenerator.GetWorld(), orgLoc, newLoc, FColor::Red, false, 100.0f, 0, 5.0f);
}

bool USectionCompProp::FindOverlaps(const ASectionGenerator& aGenerator, const APropGroup& aGroupObject, const FTransform& aTrans) const
{
	// Try again if overlap
	if (const auto volumeComp = aGroupObject.GetVolume())
	{
		FCollisionShape shape;
		if (const auto box = Cast<UBoxComponent>(volumeComp))
			shape = FCollisionShape::MakeBox(box->GetUnscaledBoxExtent() * box->GetRelativeScale3D());
		else if (const auto sphere = Cast<USphereComponent>(volumeComp))
			shape = FCollisionShape::MakeSphere(sphere->GetScaledSphereRadius());
		else if (const auto capsule = Cast<UCapsuleComponent>(volumeComp))
			shape = FCollisionShape::MakeCapsule(capsule->GetScaledCapsuleRadius(), capsule->GetScaledCapsuleHalfHeight());
		else
		{
			LOG("Unknown volume type");
			return false;
		}

		auto compTrans = FTransform(
			volumeComp->GetRelativeRotation(),
			volumeComp->GetRelativeLocation());
		auto trans = compTrans * aTrans;
		
		//DrawDebugBox(aGenerator.GetWorld(), trans.GetLocation(), shape.GetExtent(), trans.GetRotation(), FColor::Emerald, true); 
		
		TArray<FOverlapResult> overlaps;
		FCollisionQueryParams params;
		params.bFindInitialOverlaps = true;
		params.bTraceComplex = false;
		if (aGenerator.GetWorld()->OverlapMultiByChannel(
			overlaps,
			trans.GetLocation(),
			trans.GetRotation(),
			ECC_WorldStatic,
			shape,
			params))
		{
			TArray<UObstacle*> obstacles; 
			for (auto& overlap : overlaps)
			{
				if (overlap.GetActor()->IsA(APropGroup::StaticClass()))
					return true;

				overlap.GetActor()->GetComponents<UObstacle>(obstacles);
				if (obstacles.Num())
				{
					LOG("Hit obstacle");
					return true;
				}
			}
		}
	}
	return false;
}
