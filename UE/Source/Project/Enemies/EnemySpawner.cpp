﻿#include "EnemySpawner.h"

#include "EnemyManager.h"
#include "Project/Utility/MainSingelton.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	UMainSingelton::GetEnemyManager().AddSpawner(this);

	switch (mySpawnReason)
	{
	case EEnemySpawnReason::ENTER_LEVEL: // TODO: Enter level
	case EEnemySpawnReason::BEGIN_PLAY:
		Spawn();
		break;
	case EEnemySpawnReason::SPAWN_SEQUENCE:
		if (myLinkedSpawner)
			myLinkedSpawner->myOnSpawnerFinished.AddDynamic(this, &AEnemySpawner::SpawnerFinished);
		break;
	case EEnemySpawnReason::CUSTOM:
	default:
		break;
	}
}

void AEnemySpawner::EndPlay(const EEndPlayReason::Type aEndPlayReason)
{
	Super::EndPlay(aEndPlayReason);
	UMainSingelton::GetEnemyManager().RemoveSpawner(this);
}

void AEnemySpawner::SpawnerFinished(AEnemySpawner* aSpawner)
{
	if (aSpawner == myLinkedSpawner)
		Spawn();
}

void AEnemySpawner::Spawn()
{
	CHECK_RETURN_LOG(myHasSpawned, "Spawner has already spawned enemies");
	
	for (auto& enemyType : myEnemiesToSpawn)
	{
		AEnemy* enemy = SpawnEnemy(enemyType);
		CHECK_CONTINUE_LOG(!enemy, "Unable to spawn enemy");
		enemy->SetSpawner(this);
		mySpawnedEnemies.Add(enemy);
	}
	
	myHasSpawned = true;
}

void AEnemySpawner::RemoveEnemy(const AEnemy* anEnemy)
{
	mySpawnedEnemies.Remove(anEnemy);
	if (mySpawnedEnemies.Num() <= 0)
	{
		myIsFinished = true;
		myOnSpawnerFinished.Broadcast(this);
	}
}

AEnemy* AEnemySpawner::SpawnEnemy(const TSubclassOf<AEnemy>& aClass)
{
	AEnemy* enemy = Cast<AEnemy>(GetWorld()->SpawnActor(aClass));
	CHECK_RETURN_LOG(!enemy, "Unable to spawn enemy", nullptr);
	enemy->SetActorLocation(GetActorLocation());
	enemy->SetActorRotation(GetActorRotation());
	return enemy;
}