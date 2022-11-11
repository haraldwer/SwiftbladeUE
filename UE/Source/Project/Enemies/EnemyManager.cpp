#include "EnemyManager.h"

#include "EnemyConfig.h"
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Project/LevelGen/LevelRand.h"

AEnemyManager::AEnemyManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyManager::Init(UEnemyConfig* aConfig)
{
	CHECK_RETURN_LOG(!aConfig, "Config null");
	myConfig = aConfig;
	InitWave(0);
}

void AEnemyManager::CheckWaveConditions()
{
	const auto config = myConfig.Get();
	CHECK_RETURN_LOG(!config, "No enemy config set");

	const int32 nextWaveIndex = myWave + 1;
	if (!config->myWaves.IsValidIndex(nextWaveIndex))
	{
		if (myEnemies.IsEmpty() && myOnEnemiesCleared.IsBound())
			myOnEnemiesCleared.Broadcast();
		return;
	}

	const auto& wave = config->myWaves[myWave];
	const float part = static_cast<float>(myEnemies.Num()) / static_cast<float>(wave.myEnemies.Num());
	const float perc = 100.0f - FMath::Clamp(part, 0.0f, 1.0f) * 100.0f;
	if (perc > wave.myRequiredPercentage)
		InitWave(nextWaveIndex);
}

void AEnemyManager::InitWave(const int32 anIndex)
{
	myWave = anIndex; 
	
	const auto config = myConfig.Get();
	CHECK_RETURN_LOG(!config, "No enemy config set");

	CHECK_RETURN(!config->myWaves.IsValidIndex(myWave));
	
	TArray<AActor*> spawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), spawners);
	const TArray<AActor*> originalSpawners = spawners;

	// Distribute enemies across spawners
	
	auto& wave = config->myWaves[myWave];
	for (auto& enemyType : wave.myEnemies)
	{
		// Get random spawner from pool
		const int32 spawnerIndex = ULevelRand::RandRange(0, spawners.Num() - 1);
		const AActor* spawner = nullptr;
		if (spawners.IsValidIndex(spawnerIndex))
		{
			spawner = spawners[spawnerIndex];
			spawners.RemoveAtSwap(spawnerIndex);
		}
		else // Default to any spawner 
		{
			const int32 orgSpawnerIndex = ULevelRand::RandRange(0, originalSpawners.Num() - 1);
			if (originalSpawners.IsValidIndex(orgSpawnerIndex))
				spawner = originalSpawners[orgSpawnerIndex];
		}
		
		CHECK_CONTINUE_LOG(!spawner, "Invalid spawner");
		const FTransform trans = spawner->GetTransform();
		
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AEnemy* enemy = Cast<AEnemy>(GetWorld()->SpawnActor(enemyType, &trans, params));
		AddEnemy(enemy); 
	}
}

void AEnemyManager::AddEnemy(AEnemy* anEnemy)
{
	myEnemies.Add(anEnemy);
}

void AEnemyManager::RemoveEnemy(const AEnemy* anEnemy)
{
	myEnemies.Remove(anEnemy);
	CheckWaveConditions();
}

bool AEnemyManager::GetIsLastEnemy() const
{
	const auto config = myConfig.Get();
	CHECK_RETURN(!config, false);
	CHECK_RETURN(myWave < config->myWaves.Num() - 1, false);
	return myEnemies.Num() == 0; 
}
