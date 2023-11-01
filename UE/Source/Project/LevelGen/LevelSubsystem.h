#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "LevelSubsystem.generated.h"

class ARoom;
class USectionComponent;
class USplineComponent;
class USectionPreset;

UCLASS()
class PROJECT_API ULevelSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:	

	static ULevelSubsystem& Get();
	
	ULevelSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION()
	void LevelLoaded();

	void GenerateLevelOrder(int32 aChapter);
	void LoadSection(int32 anArenaIndex);
	void LoadArena(int32 anArenaIndex);
	
	float GetLowestEnd() const { return myLowestEnd; }
	USplineComponent* GetPathSpline() const { return myPathSpline.Get(); }

	ARoom* GetRoom(int32 anIndex) const;
	int32 FindRoomIndex(const ARoom& aRoom) const;

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
		TWeakObjectPtr<ARoom> myRoom; 
	};
	TArray<FLoadedLevel> myLoadedLevels; 
	
	float myLowestEnd = 0.0f;
	bool mySpawnEnemies = false;
	
	UPROPERTY()
	TObjectPtr<USplineComponent> myPathSpline;
	
};