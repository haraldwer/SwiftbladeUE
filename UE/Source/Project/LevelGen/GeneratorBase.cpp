#include "GeneratorBase.h"

#include "DrawDebugHelpers.h"
#include "GeneratorCompBase.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Utility/EngineUtility.h"

AGeneratorBase::AGeneratorBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGeneratorBase::BeginPlay()
{
	Super::BeginPlay();

	if (UEngineUtility::IsInBaseLevel())
		Generate(); 
}

void AGeneratorBase::Generate()
{
	Clear();
	FlushPersistentDebugLines(GetWorld());
	
	// Set seed
	if (mySeed)
		FMath::RandInit(mySeed);
}

void AGeneratorBase::Clear()
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

void AGeneratorBase::AddGeneratedObject(UObject* anObject)
{
	CHECK_RETURN_LOG(!anObject, "Invalid object");
	myGeneratedObjects.FindOrAdd(anObject->GetClass()).Add(anObject);
}

AActor* AGeneratorBase::SpawnGeneratedActor(const TSubclassOf<AActor>& anActorType, const FTransform& aTrans)
{
	if (AActor* actor = GetWorld()->SpawnActor(anActorType, &aTrans))
	{
		AddGeneratedObject(actor);
		return actor;
	}
	return nullptr;
}
