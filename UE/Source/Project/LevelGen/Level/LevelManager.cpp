#include "LevelManager.h"

#include "LevelEnd.h"
#include "Components/LightComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Project/LevelGen/Section/SectionGenerator.h"

ALevelManager::ALevelManager()
{
	PrimaryActorTick.bCanEverTick = false;
	myPathSpline = CreateDefaultSubobject<USplineComponent>("PathSpline");
}

void ALevelManager::GenerateLevelOrder()
{
	myLevels.Reset();
	myLevels.Add("SL_Start_0"); // Start of game
	myLevels.Add("SL_Proc_0");
	for (int i = 0; i < myNumbArenas; i++)
	{
		const FString str = "_" + FString::FromInt(i);
		
		myLevels.Add("SL_Section_End" + str);
		myArenaIndices.Add(myLevels.Num());
		myLevels.Add("SL_Arena" + str);
		myLevels.Add("SL_Section_Start" + str);
		myLevels.Add("SL_Proc" + str); 
	}
	myLevels.Add("SL_End_0"); // End of game
}

void ALevelManager::LoadSection(const int anArenaIndex)
{
	CHECK_RETURN_LOG(anArenaIndex < 0, "Arena index out of range");
	
	// If out of range, choose last index as end
	const int32 endIndex = anArenaIndex < myArenaIndices.Num() ? myArenaIndices[anArenaIndex] : myLevels.Num();
	CHECK_RETURN_LOG(endIndex < 0 || endIndex > myLevels.Num(), "Level index out of range");

	// If first arena, choose zero as start 
	const int32 startIndex = anArenaIndex > 0 ? myArenaIndices[anArenaIndex - 1] + 1 : 0;
	CHECK_RETURN_LOG(startIndex < 0 || startIndex > myLevels.Num(), "Level index out of range");
	
	// Load all levels before arena index
	TArray<int32> levelsToLoad;
	for (int32 i = startIndex; i < endIndex; i++)
		levelsToLoad.Add(i);
	
	LoadLevels(levelsToLoad);
}

void ALevelManager::LoadArena(const int anArenaIndex)
{
	CHECK_RETURN_LOG(anArenaIndex < 0 || anArenaIndex >= myArenaIndices.Num(), "Arena index out of range");
	const int32 index = myArenaIndices[anArenaIndex];
	CHECK_RETURN_LOG(index < 0 || index >= myLevels.Num(), "Level index out of range");
	LoadLevels({ index });
}

void ALevelManager::LoadLevels(const TArray<int32>& someLevelsToLoad)
{
	myLevelsToLoad.Insert(someLevelsToLoad, myLevelsToLoad.Num());
	LoadNextLevel();
}

void ALevelManager::LoadNextLevel()
{
	CHECK_RETURN(!myLevelsToLoad.Num());
	
	FLatentActionInfo loadInfo;
	loadInfo.CallbackTarget = this;
	loadInfo.ExecutionFunction = "LevelLoaded";
	loadInfo.Linkage = 0;

	const int32 levelIndex = myLevelsToLoad[0];
	myLevelsToLoad.RemoveAt(0);
	
	const FString name = myLevels[levelIndex];
	const FString choppedName = ChopLevelName(name);
	CHECK_RETURN_LOG(choppedName.IsEmpty(), "Invalid name");

	LOG("Loading level " + name);

	// First apply transform
	if (myLoadedLevels.Num())
	{
		if (const auto streamingLevel = UGameplayStatics::GetStreamingLevel(this, *choppedName))
			streamingLevel->LevelTransform = myLoadedLevels.Last().myEndTransform;
	}
	
	loadInfo.UUID = levelIndex;
	UGameplayStatics::LoadStreamLevel(this, *choppedName, true, true, loadInfo);
}

void ALevelManager::LevelLoaded()
{
	LOG("Level loaded")

	ULevelStreaming* pendingLevel = myPendingLevel.Get();
	CHECK_RETURN_LOG(!pendingLevel, "Invalid pending level on loaded");
	
	// Add level
	FLoadedLevel& loadedLevel = myLoadedLevels.Emplace_GetRef();
	loadedLevel.myStreamingLevel = pendingLevel;

	auto level = pendingLevel->GetLoadedLevel(); 
	loadedLevel.myLevel = level; 

	// Figure out end transform

	// Some rooms use levelend, some use room
	const ALevelEnd* levelEnd = nullptr;
	if (AActor** levelEndPtr = level->Actors.FindByPredicate([](const AActor* aActor)
		{ return aActor->IsA(ALevelEnd::StaticClass()); }))
		levelEnd = Cast<ALevelEnd>(*levelEndPtr);
		
	if (levelEnd)
	{
		previousPosition = levelEnd->GetActorLocation();
		if (previousPosition.Z < myLowestEnd)
			myLowestEnd = previousPosition.Z;
	}
	else LOG("Missing end location for level " + loadedLevel$.myName);
	
	loadedLevel.myEndTransform = 
	
	if (myLevelsToLoad.Num())
	{
		// try load next level
		LoadNextLevel();
	}
	else
	{
		// Set up all loaded levels
		SetupLevels();
		OptimizeObjectRendering();		
	}
}

FString ALevelManager::ChopLevelName(const FString& aName)
{
	int32 chopIndex = 0;
	aName.FindLastChar('_', chopIndex);
	return aName.Left(chopIndex); // Chop this
}

void ALevelManager::SetupLevels()
{
	LOG("Setting up levels");
	
	const auto& streamingLevels = GetWorld()->GetStreamingLevels();
	for(auto& level : streamingLevels)
	{
		CHECK_CONTINUE(!level->ShouldBeLoaded());
		CHECK_CONTINUE_LOG(!level->IsLevelLoaded(), "Level has not finished loading yet");
		const auto loadedLevel = level->GetLoadedLevel();
		CHECK_CONTINUE(!loadedLevel);

		const FString levelName = loadedLevel->GetOuter()->GetName();
		LoadedLevelData* data = myLoadedLevels.FindByPredicate([&](const LoadedLevelData& someData)
		{
			const FString chopped = ChopLevelName(someData.myName);
			return chopped == levelName;
		});
		
		CHECK_CONTINUE_LOG(!data, "Invalid streaming level");
		data->myPtr = loadedLevel;
	}

	// Sort by index
	myLoadedLevels.Sort([](const LoadedLevelData& aFirst, const LoadedLevelData& aSecond){ return aFirst.myIndex < aSecond.myIndex; });

	if (const auto pathSpline = myPathSpline.Get())
		pathSpline->ClearSplinePoints();
	
	myLowestEnd = 0.0f;
	FVector previousPosition = FVector(0, 0, 0);
	for (auto& level : myLoadedLevels)
	{
		const auto ptr = level.myPtr.Get();
		CHECK_RETURN(!ptr);

		// Set offset
		ptr->ApplyWorldOffset(previousPosition, false);
		level.myOffset = previousPosition;

		const ALevelEnd* levelEnd = nullptr;
		
		// Generate
		if (AActor** generator = ptr->Actors.FindByPredicate([](const AActor* aActor) { return aActor->IsA(AGeneratorBase::StaticClass()); }))
		{
			if (const auto genPtr = Cast<AGeneratorBase>(*generator))
			{
				genPtr->Generate(this);
				if (const auto sectionGen = Cast<ASectionGenerator>(genPtr))
					levelEnd = sectionGen->GetLevelEnd();
			}
		}
		
		// Get level end
		if (!levelEnd)
			if (AActor** levelEndPtr = ptr->Actors.FindByPredicate([](const AActor* aActor) { return aActor->IsA(ALevelEnd::StaticClass()); }))
				levelEnd = Cast<ALevelEnd>(*levelEndPtr);
		
		if (levelEnd)
		{
			previousPosition = levelEnd->GetActorLocation();
			if (previousPosition.Z < myLowestEnd)
				myLowestEnd = previousPosition.Z;
		}
		else LOG("Missing end location for level " + level.myName);
	}
}

void ALevelManager::OptimizeObjectRendering() const
{
	LOG("Setting draw distance for all primitives");
	
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);
	for(const auto& it : actors)
	{
		CHECK_CONTINUE(!it);
		TArray<UPrimitiveComponent*> comps;
		it->GetComponents<UPrimitiveComponent>(comps);
		for (const auto& comp : comps)
			if (comp) comp->SetCullDistance(myMeshDistance);

		TArray<ULightComponent*> lights;
		it->GetComponents<ULightComponent>(lights);
		for (const auto& comp : lights)
		{
			if (comp)
			{
				comp->MaxDrawDistance = myLightDistance;
				comp->MaxDistanceFadeRange = myLightDistance * 0.2f;
				comp->MarkRenderStateDirty();
			}
		}
	}
}