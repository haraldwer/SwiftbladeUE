#include "PropGenerator.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Project/Gameplay/Obstacle.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Props/PropConfig.h"
#include "Project/LevelGen/Props/PropGroup.h"
#include "Project/Utility/GameUtility.h"

APropGenerator::APropGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	
	mySpline = CreateDefaultSubobject<USplineComponent>("Spline");
	mySpline->SetMobility(EComponentMobility::Static);
	SetRootComponent(mySpline);

	myTop = CreateDefaultSubobject<UArrowComponent>("Top");
	myTop->SetMobility(EComponentMobility::Static);
	myTop->SetupAttachment(RootComponent);
}

void APropGenerator::BeginPlay()
{
	Super::BeginPlay();
	if (!UGameUtility::IsInBaseLevel())
		Generate(nullptr); 
}

void APropGenerator::Generate(ALevelManager* aLevelManager)
{
	Super::Generate(aLevelManager);
	
	CalculateArea(); 
	
	// Select random config
	const int32 confI = ULevelRand::RandRange(0, myConfigs.Num() - 1);
	const auto configType = myConfigs[confI].Get();
	CHECK_RETURN_LOG(!configType, "ConfigType nullptr");
	const auto config = configType->GetDefaultObject<UPropConfig>(); 
	CHECK_RETURN_LOG(!config, "Config nullptr");

	// Fill rate should consider area
	const float area = myArea.myRadius;
	const float countFloat = config->myFillRate * area * 0.001f;
	const int32 count = FMath::RoundToInt32(countFloat);
	for (int groupIndex = 0; groupIndex < count; groupIndex++)
		CreateGroup(*config);
}

void APropGenerator::CreateGroup(UPropConfig& aConfig)
{
	const FPropEntry* selected = SelectPropGroup(aConfig);

	CHECK_RETURN_LOG(!selected, "No selected prop");
	const auto group = selected->myGroup.Get();
	CHECK_RETURN_LOG(!group, "Group type nullptr");
	const auto defaultObject = group->GetDefaultObject<APropGroup>();
	CHECK_RETURN_LOG(!defaultObject, "Default object nullptr");

	// Try multiple times
	for (int placementIndex = 0; placementIndex < 3; placementIndex++)
	{
		FVector2D loc = GetPlacement(*defaultObject);
		FVector2D dir = (loc - GetCenter()).GetSafeNormal();

		// Adjust for walls and stuff
		AdjustPlacement(*defaultObject, loc, dir); 
		
		// Create transform
		const float yaw = FMath::RadiansToDegrees(FMath::Atan2(dir.Y, dir.X));
		const float yawRot = defaultObject->GetYawRot();
		const float rotOffset = ULevelRand::FRandRange(-yawRot * 0.5f, yawRot * 0.5f);
		
		const float height = ULevelRand::FRandRange(defaultObject->GetMinHeight(), defaultObject->GetMaxHeight());
		const float ceil = myTop->GetRelativeLocation().Z - defaultObject->GetMinCeilDist();
		const float maxHeight = FMath::Min(defaultObject->GetMaxHeight(), ceil);
		const float clampedHeight = FMath::Clamp(height, defaultObject->GetMinHeight(), maxHeight);
		const float zPos = GetActorLocation().Z + clampedHeight;
		
		const FTransform trans = FTransform(FRotator(0.0f, yaw + rotOffset + 90.0f, 0.0f), FVector(loc.X, loc.Y, zPos));

		// Avoid overlaps with existing stuff
		if (FindOverlaps(*defaultObject, trans))
			continue;
		
		// Valid placement, spawn group and generate!
		if (const auto actor = Cast<APropGroup>(SpawnGeneratedActor(group, trans)))
			actor->Generate(this);
			
		return;
	}
}

FPropEntry* APropGenerator::SelectPropGroup(UPropConfig& aConfig)
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

FVector2D APropGenerator::GetPlacement(const APropGroup& aGroupObject) const
{
	const auto spline = mySpline.Get();
	CHECK_RETURN(!spline, FVector2D::ZeroVector); 

	FVector2D loc2D = FVector2D::ZeroVector;
	switch (aGroupObject.GetPlacementMode())
	{
	case EPropPlacementMode::WALL:
		{
			const float length = spline->GetSplineLength();
			const float part = ULevelRand::FRandRange(0.0f, length);
			const FVector loc = spline->GetLocationAtDistanceAlongSpline(part, ESplineCoordinateSpace::World);
			loc2D = FVector2D(loc.X, loc.Y);
		}
		break;
		
	case EPropPlacementMode::CORNER:
		{
			const int32 numPoints = spline->GetNumberOfSplinePoints(); 
			const int32 pointI = ULevelRand::RandRange(0, numPoints);
			const FVector point = spline->GetLocationAtSplinePoint(pointI, ESplineCoordinateSpace::World);
			loc2D = FVector2D(point.X, point.Y);
		}
		break;
	}
	const auto center = GetCenter(); 
	const float currDist = FVector2D::Distance(center, loc2D); 
	const float dist = ULevelRand::FRandRange(aGroupObject.GetMinWallDist(), aGroupObject.GetMaxWallDist());
	return FMath::Lerp(loc2D, center, dist / currDist);
}

void APropGenerator::AdjustPlacement(const APropGroup& aGroupObject, FVector2D& aPlacement, FVector2D& aNormal)
{
	// Avoid corners
	const float cornerDist = aGroupObject.GetCornerDist();
	if (cornerDist > SMALL_NUMBER)
	{
		for (auto& corner : myArea.myCorners)
		{
			const FVector2D diff = aPlacement - corner;
			const float dist = diff.Size();
			if (dist < cornerDist)
				aPlacement = corner + diff.GetSafeNormal() * cornerDist;
		}
	}

	const FVector2D center = GetCenter();
	const float z = GetActorLocation().Z;
	for (int j = 0; j < mySplinePlacementPrecision; j++)
	{
		if (const auto spline = mySpline.Get())
		{
			const FVector splineLocation = spline->FindLocationClosestToWorldLocation(FVector(aPlacement.X, aPlacement.Y, z), ESplineCoordinateSpace::World);
			const FVector2D loc2D = FVector2D(splineLocation.X, splineLocation.Y);

			const FVector2D direction = (aPlacement - center).GetSafeNormal();
			const float wallOffset = aGroupObject.GetWallOffset();
			const FVector2D edgeLoc = aPlacement + direction * wallOffset;

			const float edgeDist = FVector2D::Distance(edgeLoc, center);
			if (edgeDist > FVector2D::Distance(loc2D, center))
			{
				aPlacement = loc2D - direction * wallOffset;
				const FVector splineDir = spline->FindTangentClosestToWorldLocation(FVector(loc2D.X, loc2D.Y, z), ESplineCoordinateSpace::World);
				aNormal = FVector2D(splineDir.X, splineDir.Y).GetRotated(-90.0f);
				if (aNormal.Dot(direction) < 0.0f)
					aNormal *= -1; 
			}
		}
	}
}

bool APropGenerator::FindOverlaps(const APropGroup& aGroupObject, const FTransform& aTrans) const
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
		
		TArray<FOverlapResult> overlaps;
		FCollisionQueryParams params;
		params.bFindInitialOverlaps = true;
		params.bTraceComplex = false;
		if (GetWorld()->OverlapMultiByChannel(
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

FVector2D APropGenerator::GetCenter() const
{
	const auto center = GetActorLocation();
	return FVector2D(center.X, center.Y);
}

void APropGenerator::CalculateArea()
{
	const auto spline = mySpline.Get();
	CHECK_RETURN(!spline);

	myArea.myVerts.Empty();
	myArea.myVerts.Reserve(mySubdivisions);
	myArea.myCorners.Empty();
	
	const float length = spline->GetSplineLength();
	const float part = length / mySubdivisions;
	for (int32 i = 0; i <= mySubdivisions; i++)
	{
		const FVector loc = spline->GetLocationAtDistanceAlongSpline(part * i, ESplineCoordinateSpace::World);
		myArea.myVerts.Add(FVector2D(loc.X, loc.Y));
	}

	for (int32 i = 0; i < spline->GetNumberOfSplinePoints(); i++)
	{
		const FVector corner = spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World); 
		myArea.myCorners.Add(FVector2D(corner.X, corner.Y));
	}

	auto center = GetCenter();
	
	myArea.myRadius = 0.0f;
	for (const auto& vert : myArea.myVerts)
	{
		const float dist = FVector2D::DistSquared(GetCenter(), vert);
		if (dist > myArea.myRadius)
			myArea.myRadius = dist; 
	}
	myArea.myRadius = FMath::Sqrt(myArea.myRadius);
}

