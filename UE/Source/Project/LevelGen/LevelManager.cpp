#include "LevelManager.h"

#include "SectionPreset.h"
#include "TimerManager.h"
#include "Components/LightComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Enemies/EnemyManager.h"
#include "Project/LevelGen/GeneratorBase.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/Room.h"
#include "Project/Utility/MainSingelton.h"

ALevelManager::ALevelManager()
{
	PrimaryActorTick.bCanEverTick = false;
	myPathSpline = CreateDefaultSubobject<USplineComponent>("PathSpline");
}

void ALevelManager::GenerateLevelOrder()
{
	myLevels.Reset();
	myLevels.Add("SL_Start_0"); // Start of game
	for (int i = 0; i < myNumbArenas + 1; i++)
	{
		const FString str = "_" + FString::FromInt(i);
		if (i > 0)
			myLevels.Add("SL_Section_Start" + str);
		
		const int32 presetIndex = mySectionIndex >= 0 ?
			mySectionIndex : ULevelRand::RandRange(0, mySectionPresets.Num() - 1);
		if (mySectionPresets.IsValidIndex(presetIndex))
		{
			const auto& preset = mySectionPresets[presetIndex];
			// TODO: Difficulty and ability filtering
			if (const auto defaultObj = preset.GetDefaultObject())
			{
				// TODO: Room weights
				TArray<FSectionPresetRoom> pool = defaultObj->myRooms;
				while (pool.Num())
				{
					const int32 index = ULevelRand::RandRange(0, pool.Num() - 1);
					myLevels.Add(pool[index].myLevelName + str);
					pool.RemoveAtSwap(index);
				}
			}
		}
		
		myLevels.Add("SL_Section_End" + str);
		const FString nextStr = FString("_") + (i > 9 ? "" : "0") + FString::FromInt(i);
		myArenaIndices.Add(myLevels.Num());
		if (i <= myNumbArenas)
			myLevels.Add("SL_Arena" + nextStr + str);
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
	mySpawnEnemies = true; 
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
	
	const int32 levelIndex = myLevelsToLoad[0];
	myLevelsToLoad.RemoveAt(0);
	
	const FString name = myLevels[levelIndex];
	const FString choppedName = ChopLevelName(name);
	CHECK_RETURN_LOG(choppedName.IsEmpty(), "Invalid name");

	LOG("Loading level " + name);

	ULevelStreaming* pendingLevel = UGameplayStatics::GetStreamingLevel(this, *choppedName);
	CHECK_RETURN_LOG(!IsValid(pendingLevel), "Failed to find streaming level" + choppedName);

	FLatentActionInfo loadInfo;
	loadInfo.CallbackTarget = this;
	loadInfo.ExecutionFunction = "LevelLoaded";
	loadInfo.Linkage = 0;
	loadInfo.UUID = 0;
	UGameplayStatics::LoadStreamLevel(this, *choppedName, true, true, loadInfo);
	
	myPendingLevel = pendingLevel;
}

FString ALevelManager::ChopLevelName(const FString& aName)
{
	int32 chopIndex = 0;
	aName.FindLastChar('_', chopIndex);
	return aName.Left(chopIndex); // Chop this
}

void ALevelManager::LevelLoaded()
{
	SetupLevel();
}

void ALevelManager::SetupLevel()
{
	ULevelStreaming* pendingLevel = myPendingLevel.Get();
	CHECK_RETURN_LOG(!pendingLevel, "Invalid pending level on loaded");
	
	// Add level
	FLoadedLevel& loadedLevel = myLoadedLevels.Emplace_GetRef();
	loadedLevel.myStreamingLevel = pendingLevel;

	ULevel* level = pendingLevel->GetLoadedLevel();
	if (IsValid(level))
	{
		loadedLevel.myLevel = level;

		// Find room
		const ARoom* room = nullptr;
		if (AActor** roomPtr = level->Actors.FindByPredicate([](const AActor* aActor)
			{ return aActor->IsA(ARoom::StaticClass()); }))
			room = Cast<ARoom>(*roomPtr);
		else LOG("Missing room for level " + level->GetName());
		
		const FTransform enterTrans = IsValid(room) ? room->GetEntry() : FTransform::Identity;
		const FTransform exitTrans = IsValid(room) ? room->GetExit() : FTransform::Identity;
		
		// Find last room exit 
		FVector lastRoomExit = FVector::ZeroVector;
		if (myLoadedLevels.Num() > 2)
			lastRoomExit = myLoadedLevels[myLoadedLevels.Num() - 2].myExitLocation;

		const FVector enterPos = lastRoomExit - enterTrans.GetLocation();
		level->ApplyWorldOffset(enterPos, false);
		LOG("Pos: " + enterPos.ToString());

		loadedLevel.myExitLocation = enterPos - enterTrans.GetLocation() + exitTrans.GetLocation(); 
		if (loadedLevel.myExitLocation.Z < myLowestEnd)
			myLowestEnd = loadedLevel.myExitLocation.Z;
		
		if (USplineComponent* roomPath = (IsValid(room) ? room->GetPath() : nullptr))
		{
			TArray<FSplinePoint> points;
			const int32 startIndex = myPathSpline->GetNumberOfSplinePoints();
		    for (int32 i = 0; i < roomPath->GetNumberOfSplinePoints(); i++)
		    {
			    FSplinePoint& point = points.Emplace_GetRef();
		    	point.Position = roomPath->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		    	point.Rotation = roomPath->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::World);
		    	point.Scale = roomPath->GetScaleAtSplinePoint(i);
		    	point.ArriveTangent = roomPath->GetArriveTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		    	point.LeaveTangent = roomPath->GetLeaveTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		    	point.InputKey = static_cast<float>(startIndex + i); 
		    }
		    myPathSpline->AddPoints(points, false); 
		}
		else LOG("Missing path in level " + level->GetName())
	}
	else LOG("Invalid level on load" + pendingLevel->GetName());
	
	if (myLevelsToLoad.Num())
	{
		// try load next level
		LoadNextLevel();
	}
	else
	{
		// Set up all loaded levels
		GenerateRooms();
		OptimizeObjectRendering();
		myPathSpline->UpdateSpline(); 

		if (mySpawnEnemies)
			UMainSingelton::GetEnemyManager().Init(nullptr);
	}
}

void ALevelManager::GenerateRooms()
{
	TArray<AActor*> generators; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGeneratorBase::StaticClass(), generators);
	for (const auto actor : generators)
		if (const auto generator = Cast<AGeneratorBase>(actor))
			generator->Generate(this); 
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
