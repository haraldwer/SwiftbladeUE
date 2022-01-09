#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemySpawner.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class PROJECT_API AEnemyManager : public AActor
{
	GENERATED_BODY()

public:
	AEnemyManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void AddEnemy(AEnemy* anEnemy);
	void RemoveEnemy(const AEnemy* anEnemy);

	void AddSpawner(AEnemySpawner* aSpawner);
	void RemoveSpawner(const AEnemySpawner* aSpawner);
	TSet<AEnemy*> GetEnemies() const;

private:
	TSet<AEnemy*> myEnemies;
	TSet<AEnemySpawner*> mySpawners;
};
