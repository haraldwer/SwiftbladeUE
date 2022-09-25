#include "SectionPropComponent.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionPropComponent::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	float totalWeight = 0.0f;
	for (const auto& prop : myProps)
		totalWeight += prop.mySpawnWeight;
	
	const int32 count = static_cast<int32>(FMath::RandRange(myMinFillRate, myMaxFillRate));
	for (int i = 0; i < count; i++)
	{
		float weight = FMath::RandRange(0.0f, totalWeight);
		const FSectionProp* selected = nullptr;
		for (const auto& prop : myProps)
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
		const float height = aRoom.groundOffset + aRoom.ceilHeight * 0.5f;
		if (aGenerator->GetWorld()->LineTraceSingleByChannel(hit,
			 FVector(start.X, start.Y, height),
			 FVector(end.X, end.Y, height),
			 ECC_WorldStatic))
		{
			// Place prop
			const FVector location = FVector(hit.Location.X, hit.Location.Y, aRoom.groundOffset);
			const FVector offset =
				FVector(dir.X, dir.Y, 0.0f) * -selected->myPadding +
				FVector::UpVector * selected->myHeight;
			FTransform trans = FTransform(
				hit.Normal.Cross(FVector::UpVector).Rotation(),
				location + offset);
			aGenerator->SpawnGeneratedActor(selected->myClass, trans);
		}
	}
}
