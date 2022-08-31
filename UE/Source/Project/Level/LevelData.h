#pragma once

#include "CoreMinimal.h"
#include "LevelDataStructs.h"
#include "GameFramework/Actor.h"
#include "LevelData.generated.h"

class ULevelDataConfig;

UCLASS()
class PROJECT_API ALevelData : public AActor
{
	GENERATED_BODY()
	
public:
	
	ALevelData();
	virtual void BeginPlay() override;

	bool GetDrawDebug() const { return myDebugDrawNoise; }
	
	UFUNCTION(BlueprintCallable)
	void AddGeneratedObject(UObject* anObject);

	UFUNCTION(BlueprintCallable)
	AActor* SpawnGeneratedActor(const TSubclassOf<AActor>& anActorType, const FTransform& aTrans);

protected:

	UPROPERTY(EditAnywhere, Category="Generation")
	bool myDebugDrawNoise = false;

	UPROPERTY(EditAnywhere, Category="Generation")
	int32 mySeed = 0;
	
	UPROPERTY(EditAnywhere, Category="Generation")
	TMap<ELevelType, TSubclassOf<ULevelDataConfig>> myConfigs;
	UPROPERTY(EditAnywhere, Category="Generation")
	ELevelType myType = ELevelType::NONE;
	
	UFUNCTION(CallInEditor, Category="Generation")
	void GenerateLevel();

	UFUNCTION(CallInEditor, Category="Generation")
	void Clear();
	
private:
	
	ULevelDataConfig* GetConfig() const;
	
	TMap<UClass*, TArray<TWeakObjectPtr<UObject>>> myGeneratedObjects;
	
};
