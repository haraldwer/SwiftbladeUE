#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

class USectionComponent;
class USplineComponent;
class USectionPreset;

UCLASS()
class PROJECT_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelManager();

	UFUNCTION()
	void LevelLoaded();

	void GenerateLevelOrder(int32 aChapter);
	void LoadSection(int32 anArenaIndex);
	void LoadArena(int32 anArenaIndex);
	
	float GetLowestEnd() const { return myLowestEnd; }
	USplineComponent* GetPathSpline() const { return myPathSpline.Get(); }

protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Optimization")
	float myMeshDistance = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category="Optimization")
	float myLightDistance = 4000.0f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USectionComponent> mySectionComp; 

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

	struct FLoadedLevel
	{
		TWeakObjectPtr<ULevelStreaming> myStreamingLevel;
		TWeakObjectPtr<ULevel> myLevel; 
		FVector myExitLocation = FVector::ZeroVector;
	};
	TArray<FLoadedLevel> myLoadedLevels; 
	
	float myLowestEnd = 0.0f;
	bool mySpawnEnemies = false;
	
	UPROPERTY()
	TObjectPtr<USplineComponent> myPathSpline;
	
};
