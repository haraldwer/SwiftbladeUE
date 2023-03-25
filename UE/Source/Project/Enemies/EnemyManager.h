#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemiesCleared);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveBegun, int32, aWave);

UCLASS()
class PROJECT_API AEnemyManager : public AActor
{
	GENERATED_BODY()

public:
	
	AEnemyManager();

	virtual void BeginPlay() override;
	void Init(class UEnemyConfig* aConfig);
	
	void CheckWaveConditions();
	void InitWave(int32 anIndex);

	void AddEnemy(AEnemy* anEnemy);
	void RemoveEnemy(const AEnemy* anEnemy);
	
	TSet<AEnemy*> GetEnemies() const { return myEnemies; }

	bool GetIsLastEnemy() const; 

	UPROPERTY(BlueprintAssignable)
	FOnEnemiesCleared myOnEnemiesCleared;

	UPROPERTY(BlueprintAssignable)
	FOnWaveBegun myOnWaveBegun; 

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEnemyConfig> myDefaultConfig; 

private:

	UPROPERTY()
	TSet<AEnemy*> myEnemies;

	TObjectPtr<UEnemyConfig> myConfig;
	int32 myWave = -1;
	
};
