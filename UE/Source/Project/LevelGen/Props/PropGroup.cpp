#include "PropGroup.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Project/LevelGen/GeneratorBase.h"
#include "Project/LevelGen/LevelRand.h"

APropGroup::APropGroup()
{
	PrimaryActorTick.bCanEverTick = false;

	myRoot = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(myRoot);
	myRoot->SetMobility(EComponentMobility::Static);
	
	myLocationParent = CreateDefaultSubobject<USceneComponent>("LocationParent");
	myLocationParent->SetupAttachment(myRoot);
	myLocationParent->SetMobility(EComponentMobility::Static);
	
	myVolume = CreateDefaultSubobject<UBoxComponent>("Volume");
	myVolume->SetupAttachment(myRoot);
	myVolume->SetMobility(EComponentMobility::Static);
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
		for (auto actor : myActorsToPlace)
			PlaceProp(aGenerator, unusedLocations, actor.GetDefaultObject());
		for (auto mesh : myMeshesToPlace)
			PlaceProp(aGenerator, unusedLocations, mesh.Get());
		break;
	}
	
	// Generate child prop groups

	TArray<UChildActorComponent*> childActorComps;
	GetComponents<UChildActorComponent>(childActorComps);
	for (const auto& childComp : childActorComps)
		if (childComp)
			if (const auto propGroup = Cast<APropGroup>(childComp->GetChildActor()))
				propGroup->Generate(aGenerator);
}

void APropGroup::PlaceProp(AGeneratorBase* aGenerator, TArray<FTransform>& someUnusedLocations, UObject* aDefaultObject)
{
	CHECK_RETURN_LOG(!aDefaultObject, "Prop nullptr")
	
	const FTransform spawnTrans = myPlacementStrategy == EPropPlacementStrategy::SCATTER ?
		GetScatterTrans() : GetRandomLocationTrans(someUnusedLocations);

	if (const auto mesh = Cast<UStaticMesh>(aDefaultObject))
	{
		if (const auto meshComp = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform::Identity, false)))
		{
			meshComp->AttachToComponent(myRoot, FAttachmentTransformRules::KeepRelativeTransform);
			meshComp->SetStaticMesh(mesh);
			meshComp->SetWorldTransform(spawnTrans);
			meshComp->SetMobility(EComponentMobility::Static);
		}
	}
	else if (const auto actor = Cast<AActor>(aDefaultObject))
	{
		if (const auto propGroup = Cast<APropGroup>(aGenerator->SpawnGeneratedActor(actor->GetClass(), spawnTrans)))
			propGroup->Generate(aGenerator);
			
	}
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
		const int32 index = ULevelRand::RandRange(0, someUnusedLocations.Num() - 1);
		const FTransform trans = someUnusedLocations[index];
		someUnusedLocations.RemoveAtSwap(index);
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
