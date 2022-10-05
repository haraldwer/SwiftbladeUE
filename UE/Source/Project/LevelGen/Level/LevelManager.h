#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Project/Player/FPController.h"
#include "LevelManager.generated.h"

UCLASS()
class PROJECT_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelManager();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void LevelLoaded();

	void GenerateLevelOrder(int aSeed);
	void LoadSection(int anArenaIndex);
	void LoadArena(int anArenaIndex);
	void LoadLevelOverride(const FString& aLevelName);
	
	float GetLowestEnd() const { return myLowestEnd; }

protected:
	
	UPROPERTY(EditDefaultsOnly)
	int myNumbArenas = 0;

	UPROPERTY(EditDefaultsOnly)
	bool myEnableOverlapEvents = false;

	UPROPERTY(EditDefaultsOnly)
	float myRenderDistance = 5000.0f; 
	
private:

	void LoadLevels(TArray<FString> someLevelsToLoad);

	// On levels loaded 
	void SetupLevels();
	void EnableOverlapEvents() const;
	void OptimizeObjectRendering() const;
	
	int FindLevelIndex(const ULevel* aLevel);
	int FindLevelIndex(const FString& aLevelName);

	TArray<FString> myLevels;
	TArray<int32> myArenaIndices;

	struct LoadedLevelData
	{
		FVector myOffset;
		FString myName;
		TWeakObjectPtr<ULevel> myPtr;
		int32 myIndex;
	};
	
	TArray<LoadedLevelData> myLoadedLevels;
	int32 myLoadCount = -1;
	float myLowestEnd = 0.0f; 
};
