#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemySpawner.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemiesCleared);

UCLASS()
class PROJECT_API AEnemyManager : public AActor
{
	GENERATED_BODY()

public:
	
	AEnemyManager();
	virtual void BeginPlay() override;

	void AddEnemy(AEnemy* anEnemy);
	void RemoveEnemy(const AEnemy* anEnemy);

	void AddSpawner(AEnemySpawner* aSpawner);
	void RemoveSpawner(const AEnemySpawner* aSpawner);
	TSet<AEnemy*> GetEnemies() const;

	UPROPERTY(BlueprintAssignable)
	FOnEnemiesCleared myOnEnemiesCleared;

private:

	UPROPERTY()
	TSet<AEnemy*> myEnemies;

	UPROPERTY()
	TSet<AEnemySpawner*> mySpawners;
	
};
