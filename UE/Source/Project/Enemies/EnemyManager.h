#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemiesCleared);

UCLASS()
class PROJECT_API AEnemyManager : public AActor
{
	GENERATED_BODY()

public:
	
	AEnemyManager();

	void Init(class UEnemyConfig* aConfig);
	
	void CheckWaveConditions();
	void InitWave(int32 anIndex);

	void AddEnemy(AEnemy* anEnemy);
	void RemoveEnemy(const AEnemy* anEnemy);
	
	TSet<AEnemy*> GetEnemies() const { return myEnemies; }

	bool GetIsLastEnemy() const; 

	UPROPERTY(BlueprintAssignable)
	FOnEnemiesCleared myOnEnemiesCleared;

private:

	UPROPERTY()
	TSet<AEnemy*> myEnemies;

	TObjectPtr<UEnemyConfig> myConfig;
	int32 myWave = -1;
	
};
