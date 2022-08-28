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

	UFUNCTION(BlueprintCallable)
	void AddGeneratedObject(UObject* anObject);
	
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

	UFUNCTION(BlueprintCallable)
	AActor* SpawnGeneratedActor(TSubclassOf<AActor> anActorType);
	
private:

	TArray<FVector2D> GeneratePoints() const;
	void ConstructFaces(TMap<int32, FLevelDataFace>& someFaces) const;
	void GenerateVertices(TMap<int32, FLevelDataFace>& someFaces) const;
	TArray<int32> GeneratePath(TMap<int32, FLevelDataFace>& someFaces) const;
	FVector AdjustConnectingFaces(const TArray<int32>& aPath, TMap<int32, FLevelDataFace>& someFaces) const;

	ULevelDataConfig* GetConfig() const;

	TMap<UClass*, TArray<TWeakObjectPtr<UObject>>> myGeneratedObjects;
	
};
