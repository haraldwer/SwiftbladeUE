#include "EnemyManager.h"

AEnemyManager::AEnemyManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyManager::AddEnemy(AEnemy* anEnemy)
{
	myEnemies.Add(anEnemy);
}

void AEnemyManager::RemoveEnemy(const AEnemy* anEnemy)
{
	myEnemies.Remove(anEnemy);
	if (!myEnemies.Num() && !mySpawners.Num() && myOnEnemiesCleared.IsBound())
		myOnEnemiesCleared.Broadcast();
}

void AEnemyManager::AddSpawner(AEnemySpawner* aSpawner)
{
	mySpawners.Add(aSpawner);
}

void AEnemyManager::RemoveSpawner(const AEnemySpawner* aSpawner)
{
	mySpawners.Remove(aSpawner);
	if (!myEnemies.Num() && !mySpawners.Num() && myOnEnemiesCleared.IsBound())
		myOnEnemiesCleared.Broadcast();
}

TSet<AEnemy*> AEnemyManager::GetEnemies() const
{
	return myEnemies;
}