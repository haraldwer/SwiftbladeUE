#include "SectionPropComponent.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

void USectionPropComponent::Populate(ASectionGenerator* aGenerator, const FProcSection& aSection)
{
	// Props
	if (!myPropConfig)
		return;

	const auto propConf = Cast<USectionPropConfig>(myPropConfig->GetDefaultObject());
	if (!propConf)
		return;

	for (auto& room : aSection.rooms)
	{
		float totalWeight = 0.0f;
		for (const auto& prop : propConf->myProps)
			totalWeight += prop.mySpawnWeight;
	
		const int32 count = static_cast<int32>(FMath::RandRange(propConf->myMinFillRate, propConf->myMaxFillRate));
		for (int i = 0; i < count; i++)
		{
			float weight = FMath::RandRange(0.0f, totalWeight);
			const FSectionProp* selected = nullptr;
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
			const auto start = room.center;
			const auto end = room.center + dir * room.radius * 1.5f;
			const float height = room.groundOffset + room.ceilHeight * 0.5f;
			if (aGenerator->GetWorld()->LineTraceSingleByChannel(hit,
				 FVector(start.X, start.Y, height),
				 FVector(end.X, end.Y, height),
				 ECC_WorldStatic))
			{
				// Place prop
				const FVector location = FVector(hit.Location.X, hit.Location.Y, room.groundOffset);
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
}
