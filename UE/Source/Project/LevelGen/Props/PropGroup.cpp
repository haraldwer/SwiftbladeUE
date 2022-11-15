#include "PropGroup.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Project/LevelGen/GeneratorBase.h"
#include "Project/LevelGen/LevelRand.h"

APropGroup::APropGroup()
{
	PrimaryActorTick.bCanEverTick = false;

	myRoot = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(myRoot);
	
	myLocationParent = CreateDefaultSubobject<USceneComponent>("LocationParent");
	myLocationParent->SetupAttachment(myRoot);
	
	myVolume = CreateDefaultSubobject<UBoxComponent>("Volume");
	myVolume->SetupAttachment(myRoot);
}

void APropGroup::Generate(AGeneratorBase* aGenerator)
{
	switch (myPlacementStrategy)
	{
	case EPropPlacementStrategy::PREDEFINED:
		// Already placed as child actors
		break;

	case EPropPlacementStrategy::SCATTER:
	case EPropPlacementStrategy::USE_LOCATIONS:
		TArray<FTransform> unusedLocations = GetLocations();
		for (auto prop : myPropsToPlace)
		{
			const FTransform spawnTrans = myPlacementStrategy == EPropPlacementStrategy::SCATTER ?
				GetScatterTrans() : GetRandomLocationTrans(unusedLocations);
			aGenerator->SpawnGeneratedActor(prop, spawnTrans);
		}
		break;
	}
	
	// Generate child prop groups
	TArray<AActor*> children;
	GetAllChildActors(children, false);
	for (const auto& child : children)
		if (const auto propGroup = Cast<APropGroup>(child))
			propGroup->Generate(aGenerator);
}

FTransform APropGroup::GetScatterTrans() const
{
	if (const auto box = Cast<UBoxComponent>(myVolume))
	{
		const FVector extent = box->GetUnscaledBoxExtent() * 0.5f;
		const FVector pos = FVector(
			ULevelRand::FRandRange(-extent.X, extent.X),
			ULevelRand::FRandRange(-extent.Y, extent.Y),
			ULevelRand::FRandRange(-extent.Z, extent.Z));
		return myVolume->GetComponentTransform() *
			FTransform(ULevelRand::RandVec().Rotation(),
				pos * FVector(1.0f, 1.0f, 0.0f)); 
	}
	if (const auto sphere = Cast<USphereComponent>(myVolume))
	{
		const FVector dir = ULevelRand::RandVec();
		const float dist = ULevelRand::FRandRange(0.0f, sphere->GetUnscaledSphereRadius());
		return myVolume->GetComponentTransform() *
			FTransform(ULevelRand::RandVec().Rotation(),
				dir * dist * FVector(1.0f, 1.0f, 0.0f));
	} 
	if (const auto capsule = Cast<UCapsuleComponent>(myVolume))
	{
		const FVector dir = ULevelRand::RandVec();
		const float dist = ULevelRand::FRandRange(0.0f, capsule->GetUnscaledCapsuleRadius());
		return myVolume->GetComponentTransform() *
			FTransform(ULevelRand::RandVec().Rotation(),
				dir * dist * FVector(1.0f, 1.0f, 0.0f));
	}
	LOG("Unknown volume type");
	return GetTransform(); 
}

FTransform APropGroup::GetRandomLocationTrans(TArray<FTransform>& someUnusedLocations) const
{
	// First select from pool
	if (someUnusedLocations.Num())
	{
		const FTransform trans = someUnusedLocations.Last();
		someUnusedLocations.RemoveAt(someUnusedLocations.Num() - 1);
		return trans; 
	}

	// Then select at random
	CHECK_RETURN_LOG(!myLocationParent, "Location parent nullptr", FTransform::Identity);
	auto children = myLocationParent->GetAttachChildren();
	CHECK_RETURN_LOG(!children.Num(), "Location parent no children", FTransform::Identity);
	const int32 index = ULevelRand::RandRange(0, children.Num() - 1);
	const auto child = children[index].Get();
	CHECK_RETURN_LOG(!child, "Child nullptr", FTransform::Identity);
	return child->GetComponentTransform();
}

TArray<FTransform> APropGroup::GetLocations() const
{
	TArray<FTransform> locations;
	if (myLocationParent)
		for (auto& child : myLocationParent->GetAttachChildren())
			if (const auto childPtr = child.Get())
				locations.Add(childPtr->GetComponentTransform());
	return locations;
}
