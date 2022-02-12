#pragma once

#include "CoreMinimal.h"

#include "LevelData.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

enum class EGeneratedObstacleType : uint8;

USTRUCT()
struct FRandomObjectContainer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> myArr;
};

UCLASS()
class PROJECT_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelGenerator();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = Game)
	void LevelLoaded();
	
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
	TMap<EGeneratedObstacleType, FRandomObjectContainer> myPlaceableActors;
	
private:
	
	void GenerateLevels(int aSeed);
	void MoveLevels();
	void GenerateObjects(const ULevel* aLevel);
	void GenerateObject(const class ALevelGeneratedObject* aSpawner);
	void EnableOverlapEvents() const;
	int FindLevelIndex(const ULevel* aLevel);
	static TArray<FString> GetLevelPool(FString aType, int aNumb);

	TArray<FString> myLoadedLevels;
	bool myStaticInvalid = false;
};
