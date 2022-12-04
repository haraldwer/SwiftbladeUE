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
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void LevelLoaded();

	void GenerateLevelOrder(int aSeed);
	void LoadSection(int anArenaIndex);
	void LoadArena(int anArenaIndex);
	
	float GetLowestEnd() const { return myLowestEnd; }
	USplineComponent* GetPathSpline() const { return myPathSpline.Get(); }

protected:
	
	UPROPERTY(EditDefaultsOnly)
	int myNumbArenas = 0;

	UPROPERTY(EditDefaultsOnly)
	bool myEnableOverlapEvents = false;

	UPROPERTY(EditDefaultsOnly)
	float myMeshDistance = 5000.0f;

	UPROPERTY(EditDefaultsOnly)
	float myLightDistance = 4000.0f; 
	
private:


	void LoadLevels(const TArray<int32>& someLevelsToLoad);

	static FString ChopLevelName(const FString& aName);

	// On levels loaded 
	void SetupLevels();
	void EnableOverlapEvents() const;
	void OptimizeObjectRendering() const;
	

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
	
	UPROPERTY()
	TObjectPtr<USplineComponent> myPathSpline; 
};
