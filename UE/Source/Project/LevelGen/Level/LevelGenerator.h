#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Project/Player/FPController.h"
#include "LevelGenerator.generated.h"

UCLASS()
class PROJECT_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelGenerator();
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
	int myNumbEasyLevels = 0;
	UPROPERTY(EditDefaultsOnly)
	int myNumbMediumLevels = 0;
	UPROPERTY(EditDefaultsOnly)
	int myNumbHardLevels = 0;
	UPROPERTY(EditDefaultsOnly)
	int myNumbArenas = 0;

	UPROPERTY(EditDefaultsOnly)
	bool myEnableOverlapEvents = false; 
	
private:

	void LoadLevels(TArray<FString> someLevelsToLoad);

	// On levels loaded 
	void SetupLevels();
	void EnableOverlapEvents() const;
	
	int FindLevelIndex(const ULevel* aLevel);
	int FindLevelIndex(const FString& aLevelName);
	static TArray<FString> GetLevelPool(FString aType, int aNumb);

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
