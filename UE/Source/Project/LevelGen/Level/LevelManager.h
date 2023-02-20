#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

class USplineComponent;

UCLASS()
class PROJECT_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelManager();

	UFUNCTION()
	void LevelLoaded();

	void GenerateLevelOrder();
	void LoadSection(int anArenaIndex);
	void LoadArena(int anArenaIndex);
	
	float GetLowestEnd() const { return myLowestEnd; }
	USplineComponent* GetPathSpline() const { return myPathSpline.Get(); }

protected:
	
	UPROPERTY(EditDefaultsOnly)
	int myNumbArenas = 2;

	UPROPERTY(EditDefaultsOnly)
	int myNumRooms = 2;

	UPROPERTY(EditDefaultsOnly)
	int myNumRoomsPerSection = 2;

	UPROPERTY(EditDefaultsOnly)
	float myMeshDistance = 5000.0f;

	UPROPERTY(EditDefaultsOnly)
	float myLightDistance = 4000.0f;

private:

	// Will not unload currently loaded levels, assuming fresh base level
	void LoadLevels(const TArray<int32>& someLevelsToLoad);
	void LoadNextLevel();

	static FString ChopLevelName(const FString& aName);

	// On levels loaded
	void SetupLevel();
	void GenerateRooms();
	void OptimizeObjectRendering() const;
	
	TArray<FString> myLevels;
	TArray<int32> myArenaIndices;
	TArray<int32> myLevelsToLoad;
	TWeakObjectPtr<ULevelStreaming> myPendingLevel;
	FTimerHandle myPendingTimer; 

	struct FLoadedLevel
	{
		TWeakObjectPtr<ULevelStreaming> myStreamingLevel;
		TWeakObjectPtr<ULevel> myLevel; 
		FVector myExitLocation = FVector::ZeroVector;
	};
	TArray<FLoadedLevel> myLoadedLevels; 
	
	float myLowestEnd = 0.0f;
	int mySpawnEnemies = false;
	
	UPROPERTY()
	TObjectPtr<USplineComponent> myPathSpline; 
};
