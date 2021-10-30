#pragma once

#include "CoreMinimal.h"

#include "LevelData.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int myNumbEasyLevels = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int myNumbMediumLevels = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int myNumbHardLevels = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int myNumbArenas = 0;

	UFUNCTION(BlueprintCallable, Category = Game)
	void LevelLoaded();
	
private:
	void GenerateLevels();
	void MoveLevels();
	void EnableOverlapEvents() const;
	int FindLevelIndex(ULevel* aLevel);
	static TArray<FString> GetLevelPool(FString aType, int aNumb);

	TArray<FString> myLoadedLevels;
	bool myStaticInvalid = false;
};
