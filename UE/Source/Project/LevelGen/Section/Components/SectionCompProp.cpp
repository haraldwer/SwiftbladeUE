#include "SectionCompProp.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Props/PropConfig.h"
#include "Project/LevelGen/Props/PropGroup.h"
#include "Project/LevelGen/Section/SectionGenerator.h"
#include "Project/Utility/Math/LineIntersection.h"

void USectionCompProp::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	CHECK_RETURN_LOG(!myConfigs.Num(), "No configs in prop comp");

	const int32 confI = ULevelRand::RandRange(0, myConfigs.Num() - 1);
	const auto configType = myConfigs[confI].Get();
	CHECK_RETURN_LOG(!configType, "ConfigType nullptr");
	const auto config = configType->GetDefaultObject<UPropConfig>(); 
	CHECK_RETURN_LOG(!config, "Config nullptr");

	float totalWeight = 0.0f;
	for (const auto& entry : config->myEntries)
		totalWeight += entry.mySpawnWeight;

	// Fill rate should consider room area
	// TODO: Use vertices to calculate
	const float roomArea = aRoom.radius;
	const float countFloat = config->myFillRate * roomArea * 0.001f;
	const int32 count = FMath::RoundToInt32(countFloat); 
	for (int groupIndex = 0; groupIndex < count; groupIndex++)
	{
		float weight = ULevelRand::FRandRange(0.0f, totalWeight);

		const FPropEntry* selected = nullptr;
		for (const auto& prop : config->myEntries)
		{
			weight -= prop.mySpawnWeight;
			if (weight <= 0.0f)
			{
				selected = &prop;
				break;
			}
		}
		
		CHECK_CONTINUE_LOG(!selected, "No selected prop");
		auto group = selected->myGroup.Get();
		CHECK_CONTINUE_LOG(!group, "Group type nullptr");
		auto defaultObject = group->GetDefaultObject<APropGroup>();
		CHECK_CONTINUE_LOG(!defaultObject, "Default object nullptr");

		// Try multiple times 
		for (int placementIndex = 0; placementIndex < 5; placementIndex++)
		{
			// Get random location in room
			const FVector randVec = ULevelRand::RandVec();
			FVector2D loc = aRoom.location +
				FVector2D(randVec.X, randVec.Y).GetSafeNormal() *
				ULevelRand::FRandRange(0.0f, aRoom.radius);
			
			// Move inwards to avoid intersects
			// Repeat to avoid moving into another close wall 
			for (int j = 0; j < 2; j++)
			{
				for (int32 vertI = 0; vertI < aRoom.vertices.Num(); vertI++)
				{
					const int32 nextVertI = (vertI + 1) % aRoom.vertices.Num();
					const FVector2D vert = aRoom.vertices[vertI];
					const FVector2D nextVert = aRoom.vertices[nextVertI];
				
					const FVector2D direction = (loc - aRoom.center).GetSafeNormal();
					const float wallOffset = defaultObject->GetWallOffset();
					const FVector2D edgeLoc = loc + direction * wallOffset; 

					static FVector2D result;
					if (LineIntersect(vert, nextVert, aRoom.center, edgeLoc, result))
						loc = result - direction * wallOffset;
				}
			}

			// TODO: Some props should be snapped to a wall
			// In that case, select random wall edge
			// TODO: Store rotation for closest wall in case of edge snapping

			// Calculate rotation
			const FVector2D dir = (loc - aRoom.center).GetSafeNormal(); 
			const float yaw = FMath::Atan2(dir.Y, dir.X);
			const float yawRot = defaultObject->GetYawRot();
			const float rotOffset = ULevelRand::FRandRange(-yawRot * 0.5f, yawRot * 0.5f);

			// Create transform
			FTransform trans = FTransform(FRotator(0.0f, yaw + rotOffset, 0.0f), FVector(loc.X, loc.Y, aRoom.groundOffset));
			
			// Try again if overlap
			if (const auto volumeComp = defaultObject->GetVolume())
			{
				FCollisionShape shape;
				if (const auto box = Cast<UBoxComponent>(volumeComp))
					shape = FCollisionShape::MakeBox(box->GetScaledBoxExtent() * 0.5f);
				else if (const auto sphere = Cast<USphereComponent>(volumeComp))
					shape = FCollisionShape::MakeSphere(sphere->GetScaledSphereRadius());
				else if (const auto capsule = Cast<UCapsuleComponent>(volumeComp))
					shape = FCollisionShape::MakeCapsule(capsule->GetScaledCapsuleRadius(), capsule->GetScaledCapsuleHalfHeight());
				else break; 
				
				TArray<FOverlapResult> overlaps;
				FCollisionQueryParams params;
				params.bFindInitialOverlaps = true;
				params.bTraceComplex = false;
				
				if (GetWorld()->OverlapMultiByChannel(
					overlaps,
					trans.GetLocation() + volumeComp->GetRelativeLocation(),
					volumeComp->GetRelativeRotation().Quaternion() * trans.GetRotation(),
					ECC_WorldStatic,
					shape,
					params))
				{
					bool foundOverlap = false;
					for (auto& overlap : overlaps)
					{
						if (overlap.GetActor()->IsA(APropGroup::StaticClass()))
						{
							foundOverlap = true;
							break;
						}
					}
					if (foundOverlap)
						continue;
				}
			}

			// Valid placement, spawn group and generate!
			if (auto actor = Cast<APropGroup>(aGenerator->SpawnGeneratedActor(group, trans)))
				actor->Generate(aGenerator);
			
			break;
		}
	}
}
