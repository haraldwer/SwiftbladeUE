#include "LevelGenerator.h"

#include "LevelEndLocation.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"

ALevelGenerator::ALevelGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
	EnableOverlapEvents();
}

void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(myStaticInvalid)
	{
		SetupLevels();
		EnableOverlapEvents();
		myStaticInvalid = false;
		SetActorTickEnabled(false);
	}
}

void ALevelGenerator::LevelLoaded()
{
	LOG("Level loaded");
	myStaticInvalid = true;
	SetActorTickEnabled(true);
}

void ALevelGenerator::GenerateLevelOrder(const int aSeed)
{
	LOG("Generating levels");
	
	FMath::RandInit(aSeed);

	TArray<FString> easy = GetLevelPool("SL_Easy", myNumbEasyLevels);
	TArray<FString> arena = GetLevelPool("SL_Arena", myNumbArenas);

	TArray<TArray<FString>> total;
	total.Add(GetLevelPool("SL_Medium", myNumbMediumLevels));
	total.Add(GetLevelPool("SL_Hard", myNumbHardLevels));

	myLevels.Reset();
	myLevels.Add("SL_Start"); // Start of game
	
	TArray<FString> pool = easy;
	while (pool.Num() != 0)
	{
		const int index = FMath::RandRange(0, pool.Num() - 1);
		myLevels.Add(pool[index]);

		bool found = false;
		for (auto& list : total)
		{
			if (list.Num() != 0)
			{
				const int listIndex = FMath::RandRange(0, list.Num() - 1);
				pool[index] = list[listIndex];
				list.RemoveAt(listIndex);
				found = true;
				break;
			}
		}
		if (!found)
			pool.RemoveAt(index);
	}

	myLevels.Add("SL_End"); // End of game

	if (arena.Num() > 0)
	{
		myArenaIndices.Reset();
		const int arenaStep = myLevels.Num() / arena.Num();
		for (int i = 0; i < arena.Num(); i++)
		{
			const int32 index = FMath::Clamp(arenaStep * (i + 1), 1, myLevels.Num() - 1);
			myLevels.Insert("SL_Section_Start", index);
			myLevels.Insert(arena[i], index);
			myLevels.Insert("SL_Section_End", index);
			myArenaIndices.Add(index + 1);		
		}
	}
}

void ALevelGenerator::LoadSection(const int anArenaIndex)
{
	CHECK_RETURN_LOG(anArenaIndex < 0, "Arena index out of range");
	
	// If out of range, choose last index as end
	const int32 endIndex = anArenaIndex < myArenaIndices.Num() ? myArenaIndices[anArenaIndex] : myLevels.Num();
	CHECK_RETURN_LOG(endIndex < 0 || endIndex > myLevels.Num(), "Level index out of range");

	// If first arena, choose zero as start 
	const int32 startIndex = anArenaIndex > 0 ? myArenaIndices[anArenaIndex - 1] + 1 : 0;
	CHECK_RETURN_LOG(startIndex < 0 || startIndex > myLevels.Num(), "Level index out of range");
	
	// Load all levels before arena index
	TArray<FString> levelsToLoad;
	for (int32 i = startIndex; i < endIndex; i++)
		levelsToLoad.Add(myLevels[i]);
	
	LoadLevels(levelsToLoad);
}

void ALevelGenerator::LoadArena(const int anArenaIndex)
{
	CHECK_RETURN_LOG(anArenaIndex < 0 || anArenaIndex >= myArenaIndices.Num(), "Arena index out of range");
	const int32 index = myArenaIndices[anArenaIndex];
	CHECK_RETURN_LOG(index < 0 || index >= myLevels.Num(), "Level index out of range");
	LoadLevels({myLevels[index]});
}

void ALevelGenerator::LoadLevelOverride(const FString& aLevelName)
{
	LoadLevels({ aLevelName });
}

void ALevelGenerator::LoadLevels(TArray<FString> someLevelsToLoad)
{
	myLoadedLevels.Reset();
	
	int uuid = 0;
	FLatentActionInfo loadInfo;
	loadInfo.CallbackTarget = this;
	loadInfo.ExecutionFunction = "LevelLoaded";
	loadInfo.Linkage = 0;
	for (auto& it : someLevelsToLoad)
	{
		const int index = FindLevelIndex(it);
		CHECK_CONTINUE_LOG(index == -1, "Invalid index");
		uuid++;
		loadInfo.UUID = uuid;
		UGameplayStatics::LoadStreamLevel(this, *it, true, true, loadInfo);

		if (const auto streamingLevel = UGameplayStatics::GetStreamingLevel(this, *it))
		{
			streamingLevel->SetShouldBeLoaded(true);
			streamingLevel->SetShouldBeVisible(true);
		}
		
		myLoadedLevels.Add(LoadedLevelData{FVector(), it, nullptr, index});
	}
}

void ALevelGenerator::SetupLevels()
{
	LOG("Setting up levels");
	
	const auto& streamingLevels = GetWorld()->GetStreamingLevels();
	for(auto& level : streamingLevels)
	{
		CHECK_CONTINUE(!level->ShouldBeLoaded());
		CHECK_CONTINUE_LOG(!level->IsLevelLoaded(), "Level has not finished loading yet");
		const auto loadedLevel = level->GetLoadedLevel();
		CHECK_CONTINUE(!loadedLevel);
		const int index = FindLevelIndex(loadedLevel);
		CHECK_CONTINUE(index == -1);
		LoadedLevelData* data = myLoadedLevels.FindByPredicate([&](const LoadedLevelData& someData)
		{
			return someData.myIndex == index;
		});
		CHECK_CONTINUE_LOG(!data, "Invalid streaming level");
		data->myPtr = loadedLevel;
	}

	// Sort by index
	myLoadedLevels.Sort([](const LoadedLevelData& aFirst, const LoadedLevelData& aSecond){ return aFirst.myIndex < aSecond.myIndex; });

	myLowestEnd = 0.0f;
	FVector previousPosition = FVector(0, 0, 0);
	for (auto& level : myLoadedLevels)
	{
		const auto ptr = level.myPtr.Get();
		CHECK_RETURN(!ptr);
		ptr->ApplyWorldOffset(previousPosition, false);
		LOG("Apply offset: " + level.myName + " " + FString::SanitizeFloat(previousPosition.X) + " Previous offset: " + FString::SanitizeFloat(level.myOffset.X));
		level.myOffset = previousPosition;
		AActor** end = ptr->Actors.FindByPredicate([](const AActor* aActor) { return aActor->IsA(ALevelEndLocation::StaticClass()); });
		if (end && *end)
		{
			previousPosition = (*end)->GetActorLocation();
			if (previousPosition.Z < myLowestEnd)
				myLowestEnd = previousPosition.Z;
		}
		else LOG("Missing end location for level " + level.myName);
	}
}

void ALevelGenerator::EnableOverlapEvents() const
{
	if (!myEnableOverlapEvents)
		return;
	
	LOG("Enabling overlap events");

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);
	for(const auto& it : actors)
	{
		if (const auto comp = Cast<UStaticMeshComponent>(it->GetComponentByClass(UStaticMeshComponent::StaticClass())))
			comp->SetGenerateOverlapEvents(true);
	}
}

int ALevelGenerator::FindLevelIndex(const ULevel* aLevel)
{
	return FindLevelIndex(aLevel->GetOuter()->GetName());
}

int ALevelGenerator::FindLevelIndex(const FString& aName)
{
	for (int32 i = 0; i < myLevels.Num(); i++)
	{
		if (myLevels[i] == aName)
			return i;
	}
	return -1; 
}

TArray<FString> ALevelGenerator::GetLevelPool(const FString aType, int aNumb)
{
	TArray<FString> vec;
	for (int i = 0; i < aNumb; i++)
	{
		FString levelName = aType + "_";
		levelName.AppendInt(i);
		vec.Add(levelName);
	}
	return vec;
}
