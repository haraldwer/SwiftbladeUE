#include "LevelDataConfig.h"

#include "DrawDebugHelpers.h"
#include "Project/Level/FaceMeshGeneration.h"

void ULevelDataConfig::Populate(ALevelData* aLevelData, const TArray<FLevelDataRoom>& someRooms)
{
	for (const auto& room : someRooms)
		PopulateRoom(aLevelData, room);
}

void ULevelDataConfig::PopulateRoom(ALevelData* aLevelData, const FLevelDataRoom& aRoom)
{
	// Create ground and ceiling
	const float groundOffset = FMath::RandRange(myGroundMinOffset, myGroundMaxOffset);
	const float ceilHeight = FMath::RandRange(myCeilingMinHeight, myCeilingMaxHeight);
	if (myHasGround)
	{
		const FVector offset = FVector(0, 0, groundOffset);
		CreateFaceMesh(aLevelData, aRoom, offset, myGroundThickness, myGroundMaterial);
	}
	if (myHasCeil)
	{
		const FVector offset = FVector(0, 0, groundOffset + ceilHeight + myCeilingThickness);
		CreateFaceMesh(aLevelData, aRoom, offset, myCeilingThickness, myCeilingMaterial);
	}

	// Create wall
	for (const auto& wall : aRoom.walls)
		if (wall.verts.Num() > 1)
			CreateWall(aLevelData, wall.verts, groundOffset, ceilHeight);

	// Props
	if (const auto propConf = Cast<ULevelPropConfig>(myPropConfig->GetDefaultObject()))
	{
		float totalWeight = 0.0f;
		for (const auto& prop : propConf->myProps)
			totalWeight += prop.mySpawnWeight;
		
		const int32 count = static_cast<int32>(FMath::RandRange(propConf->myMinFillRate, propConf->myMaxFillRate));
		for (int i = 0; i < count; i++)
		{
			float weight = FMath::RandRange(0.0f, totalWeight);
			const FLevelProp* selected = nullptr;
			for (const auto& prop : propConf->myProps)
			{
				weight -= prop.mySpawnWeight;
				if (weight <= 0.0f)
				{
					selected = &prop;
					break;
				}
			}
			CHECK_CONTINUE_LOG(!selected, "No selected prop");
			
			// Check trace
			FHitResult hit;
			const auto dir = FMath::RandPointInCircle(1.0f).GetSafeNormal();
			const auto start = aRoom.center;
			const auto end = aRoom.center + dir * aRoom.radius * 1.5f;
			const float height = groundOffset + ceilHeight * 0.5f;
			if (aLevelData->GetWorld()->LineTraceSingleByChannel(hit,
				FVector(start.X, start.Y, height),
				FVector(end.X, end.Y, height),
				ECollisionChannel::ECC_WorldStatic))
			{
				// Place prop
				const FVector location = FVector(hit.Location.X, hit.Location.Y, groundOffset);
				const FVector offset =
					FVector(dir.X, dir.Y, 0.0f) * -selected->myPadding +
					FVector::UpVector * selected->myHeight;
				FTransform trans = FTransform(
					hit.Normal.Cross(FVector::UpVector).Rotation(),
					location + offset);
				AActor* actor = aLevelData->SpawnGeneratedActor(selected->myClass, trans);
			}
		}
	}	
}
