#include "Room.h"

#include "LevelRand.h"

ARoom::ARoom()
{
	PrimaryActorTick.bCanEverTick = false;

	myRoot = CreateDefaultSubobject<USceneComponent>("Root");
	myRoot->SetMobility(EComponentMobility::Static);
	SetRootComponent(myRoot); 
	
	myEntry = CreateDefaultSubobject<USceneComponent>("Entry");
	myEntry->SetupAttachment(myRoot);
	myEntry->SetMobility(EComponentMobility::Static);
	myEntryMesh = CreateDefaultSubobject<UBoxComponent>("EntryMesh");
	myEntryMesh->SetupAttachment(myEntry);
	myEntryMesh->SetMobility(EComponentMobility::Static);
	myEntryMesh->SetBoxExtent(FVector::OneVector);
	myEntryMesh->SetRelativeScale3D(FVector(1.0f, myConnectionWidth * 0.5f, 1.0f));
	myEntryMesh->SetCollisionObjectType(ECC_WorldStatic);
	myEntryMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	myEntryMesh->SetCollisionProfileName("NoCollision");
	
	myExit = CreateDefaultSubobject<USceneComponent>("Exit");
	myExit->SetupAttachment(myRoot);
	myExit->SetMobility(EComponentMobility::Static);
	myExitMesh = CreateDefaultSubobject<UBoxComponent>("ExitMesh");
	myExitMesh->SetupAttachment(myExit);
	myExitMesh->SetMobility(EComponentMobility::Static);
	myExitMesh->SetBoxExtent(FVector::OneVector); 
	myExitMesh->SetRelativeScale3D(FVector(1.0f, myConnectionWidth * 0.5f, 1.0f));
	myExitMesh->SetCollisionObjectType(ECC_WorldStatic);
	myExitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	myExitMesh->SetCollisionProfileName("NoCollision"); 
	
	myPath = CreateDefaultSubobject<USplineComponent>("Path");
	myPath->SetupAttachment(myRoot);
	myPath->SetMobility(EComponentMobility::Static);
}

void ARoom::BeginPlay()
{
	Super::BeginPlay();
	Generate(nullptr); 
}

void ARoom::Generate(ALevelManager* aLevelManager)
{
	Super::Generate(aLevelManager);
	
	CHECK_RETURN_LOG(!myPresets.Num(), "No presets for room " + GetLevel()->GetName()); 
	
	TSet<AActor*> affectedActors;
	TSet<AActor*> enabledActors;
	
	int32 index = myPresetIndex;

	constexpr int32 iterations = 32; 
	for (int32 i = 0; i < iterations && index == -1; i++)
	{
		const int32 tryIndex = ULevelRand::RandRange(0, myPresets.Num() - 1);
		CHECK_CONTINUE(!myPresets.IsValidIndex(tryIndex));
		auto& preset = myPresets[tryIndex];

		// TODO: Filtering
		//preset.myDifficuly;
		//preset.myRequiredAbilities

		// Compare abilities and difficulty
		
		index = tryIndex;
	}

	CHECK_RETURN_LOG(!myPresets.IsValidIndex(index), "Failed to select a valid room preset in room " + GetLevel()->GetName());

	for (auto& preset : myPresets)
		for (auto& actor : preset.myActors)
			affectedActors.Add(actor.Get());
	
	auto& currentPreset = myPresets[index];
	for (auto& actor : currentPreset.myActors)
		enabledActors.Add(actor.Get());
	
	for (const auto& actor : affectedActors)
	{
		CHECK_CONTINUE(!actor); 
		if (!enabledActors.Contains(actor))
		{
			actor->SetActorHiddenInGame(true);
			actor->SetActorEnableCollision(false);
			TArray<UPrimitiveComponent*> primitives;
			actor->GetComponents(primitives, true);
			for (const auto comp : primitives)
				comp->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
			
		}
	}
}

void ARoom::UpdatePath() const
{
	const auto entry = myEntry.Get();
	CHECK_RETURN(!entry);
	const auto exit = myExit.Get();
	CHECK_RETURN(!exit);
	const auto path = myPath.Get();
	CHECK_RETURN(!path)
	
	if (path->GetNumberOfSplinePoints() > 1)
	{
		const auto splineStart = path->GetTransformAtSplinePoint(0, ESplineCoordinateSpace::World);
		const auto entryTrans = entry->GetComponentTransform();
		if (splineStart.GetLocation() != entryTrans.GetLocation() ||
			splineStart.GetRotation() != entryTrans.GetRotation())
		{
			path->SetLocationAtSplinePoint(0, entryTrans.GetLocation(), ESplineCoordinateSpace::World, false);
			path->SetRotationAtSplinePoint(0, entryTrans.GetRotation().Rotator(), ESplineCoordinateSpace::World, false);
			path->SetTangentAtSplinePoint(0, entryTrans.GetRotation().Vector() * 500.0f, ESplineCoordinateSpace::World, false);
		}

		const auto splineEnd = path->GetTransformAtSplinePoint(path->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
		const auto exitTrans = exit->GetComponentTransform();
		if (splineEnd.GetLocation() != exitTrans.GetLocation() ||
			splineEnd.GetRotation() != exitTrans.GetRotation())
		{
			path->SetLocationAtSplinePoint(path->GetNumberOfSplinePoints() - 1, exitTrans.GetLocation(), ESplineCoordinateSpace::World, false);
			path->SetRotationAtSplinePoint(path->GetNumberOfSplinePoints() - 1, exitTrans.GetRotation().Rotator(), ESplineCoordinateSpace::World, false);
			path->SetTangentAtSplinePoint(path->GetNumberOfSplinePoints() - 1, exitTrans.GetRotation().Vector() * 500.0f, ESplineCoordinateSpace::World, false);
		}
		path->UpdateSpline();
	}
	else
	{
		path->ClearSplinePoints();
		path->AddSplinePoint(entry->GetComponentLocation(), ESplineCoordinateSpace::World);
		path->SetRotationAtSplinePoint(0, entry->GetComponentRotation(), ESplineCoordinateSpace::World);
		path->SetTangentAtSplinePoint(0, entry->GetComponentRotation().Vector() * 500.0f, ESplineCoordinateSpace::World);
		path->AddSplinePoint(exit->GetComponentLocation(), ESplineCoordinateSpace::World);
		path->SetRotationAtSplinePoint(1, exit->GetComponentRotation(), ESplineCoordinateSpace::World);
		path->SetTangentAtSplinePoint(1, exit->GetComponentRotation().Vector() * 500.0f, ESplineCoordinateSpace::World);
	}
}

FTransform ARoom::GetEntry() const
{
	const auto entry = myEntry.Get();
	CHECK_RETURN(!entry, FTransform::Identity);
	return entry->GetComponentTransform();
}

FTransform ARoom::GetExit() const
{
	const auto exit = myExit.Get();
	CHECK_RETURN(!exit, FTransform::Identity);
	return exit->GetComponentTransform(); 
}