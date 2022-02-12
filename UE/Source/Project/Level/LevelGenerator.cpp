#include "LevelGenerator.h"

#include "LevelEndLocation.h"
#include "LevelGeneratedObject.h"
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
	GenerateLevels(10);
}

void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(myStaticInvalid)
	{
		MoveLevels();
		myStaticInvalid = false;
	}
}

void ALevelGenerator::LevelLoaded()
{
	LOG("Level loaded");
	myStaticInvalid = true;
}

void ALevelGenerator::GenerateLevels(int aSeed)
{
	LOG("Generating levels");
	
	FMath::RandInit(aSeed);

	TArray<FString> easy = GetLevelPool("Easy", myNumbEasyLevels);
	TArray<FString> arena = GetLevelPool("Arena", myNumbArenas);

	TArray<TArray<FString>> total;
	total.Add(GetLevelPool("Medium", myNumbMediumLevels));
	total.Add(GetLevelPool("Hard", myNumbHardLevels));

	myLoadedLevels.Empty();
	TArray<FString> pool = easy;

	while (pool.Num() != 0)
	{
		const int index = FMath::RandRange(0, pool.Num() - 1);
		myLoadedLevels.Add(pool[index]);

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

	if (arena.Num() > 0)
	{
		const int arenaStep = myLoadedLevels.Num() / arena.Num();
		for (int i = 0; i < arena.Num(); i++)
			myLoadedLevels.Insert(arena[i], FMath::Clamp(arenaStep * (i + 1) + i, 1, arena.Num()));
	}

	int uuid = 0;
	FLatentActionInfo loadInfo;
	loadInfo.CallbackTarget = this;
	loadInfo.ExecutionFunction = "LevelLoaded";
	loadInfo.Linkage = 0;
	for (auto& it : myLoadedLevels)
	{
		uuid++;
		loadInfo.UUID = uuid;
		UGameplayStatics::LoadStreamLevel(this, *it, true, true, loadInfo);
	}
}

void ALevelGenerator::MoveLevels()
{
	LOG("Moving levels");

	FVector previousPosition = FVector(0, 0, 0);
	const auto& streamingLevels = GetWorld()->GetStreamingLevels();
	for(auto& level : streamingLevels)
	{
		const auto loadedLevel = level->GetLoadedLevel();
		if (!loadedLevel)
			continue;

		GenerateObjects(loadedLevel);
		
		const int index = FindLevelIndex(loadedLevel);
		if (index == -1)
			continue;
		loadedLevel->ApplyWorldOffset(previousPosition, false);
		AActor** end = loadedLevel->Actors.FindByPredicate([](AActor* actor) { return actor->IsA(ALevelEndLocation::StaticClass()); });
		if (end && *end)
			previousPosition = (*end)->GetActorLocation();
		else
			LOG("Missing end location for level");
	}
}

void ALevelGenerator::GenerateObjects(const ULevel* aLevel)
{
	for (auto& actor : aLevel->Actors)
	{
		const auto spawner = Cast<ALevelGeneratedObject>(actor);
		if (!spawner)
			continue;
		GenerateObject(spawner);
	}
}

void ALevelGenerator::GenerateObject(const ALevelGeneratedObject* aSpawner)
{
	const auto data = aSpawner->GetData();
	CHECK_RETURN_LOG(data.myTypes.Num(), "LevelGeneratedObject has no types");
	TArray<EGeneratedObstacleType> typeArr;
	for (auto& it : data.myTypes)
		typeArr.Add(it);
	const auto typeEnum = typeArr[FMath::RandRange(0, data.myTypes.Num())];
	const auto arr = myPlaceableActors.Find(typeEnum);
	CHECK_RETURN_LOG(arr, "No array for spawner type");
	const auto num = arr->myArr.Num();
	CHECK_RETURN_LOG(num, "No actors in array");
	const auto index = FMath::RandRange(0, num);
	const auto type = arr->myArr[index];
	const auto actor = GetWorld()->SpawnActor(type.Get());
	actor->SetActorTransform(aSpawner->GetTransform());
}

void ALevelGenerator::EnableOverlapEvents() const
{
	LOG("Enabling overlap events");

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);
	for(auto& it : actors)
	{
		const auto comp = it->GetComponentByClass(UStaticMeshComponent::StaticClass());
		if (!comp)
			continue;
		const auto staticMesh = Cast<UStaticMeshComponent>(comp);
		staticMesh->SetGenerateOverlapEvents(true);
	}
}

int ALevelGenerator::FindLevelIndex(const ULevel* aLevel)
{
	const auto name = aLevel->GetOuter()->GetName();
	int j = 0;
	for (auto& it : myLoadedLevels)
	{
		if (it == name)
			return j;
		j++;
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
