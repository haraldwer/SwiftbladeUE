#include "EnemyManager.h"

AEnemyManager::AEnemyManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyManager::AddEnemy(AEnemy* anEnemy)
{
	myEnemies.Add(anEnemy);
}

void AEnemyManager::RemoveEnemy(const AEnemy* anEnemy)
{
	myEnemies.Remove(anEnemy);
}

void AEnemyManager::AddSpawner(AEnemySpawner* aSpawner)
{
	mySpawners.Add(aSpawner);
}

void AEnemyManager::RemoveSpawner(const AEnemySpawner* aSpawner)
{
	mySpawners.Remove(aSpawner);
}

TSet<AEnemy*> AEnemyManager::GetEnemies() const
{
	return myEnemies;
}
