#pragma once

#include "CoreMinimal.h"
#include "ArenaConfig.generated.h"

class ACheckpoint;
class UArenaCompBase;

USTRUCT()
struct FArenaConfigLayer
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	float myMinRadius = 500.0f;
	UPROPERTY(EditDefaultsOnly)
	float myMaxRadius = 500.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myMinSubHeight = 0.0f;
	UPROPERTY(EditDefaultsOnly)
	float myMaxSubHeight = 0.0f;
	
	UPROPERTY(Instanced, EditDefaultsOnly)
	TArray<TObjectPtr<UArenaCompBase>> myComponents;
	
};

UCLASS(Blueprintable)
class PROJECT_API UArenaConfig : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACheckpoint> myCheckpointClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> myDoorClass;

	UPROPERTY(EditDefaultsOnly)
	int32 mySubdivisions = 2;
	
	UPROPERTY(EditDefaultsOnly)
	int32 myMinSections = 2;
	
	UPROPERTY(EditDefaultsOnly)
	int32 myMaxSections = 6;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FArenaConfigLayer> myLayerConfigs;

	UPROPERTY(EditDefaultsOnly)
	float myDoorHeight = 500.0f;

	UPROPERTY(EditDefaultsOnly)
	float myDoorRadiusPart = 0.8f;

	UPROPERTY(Instanced, EditDefaultsOnly)
	TObjectPtr<class UEnemyConfig> myEnemyConfig;
};
