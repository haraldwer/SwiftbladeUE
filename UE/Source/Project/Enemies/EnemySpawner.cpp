#include "EnemySpawner.h"

#include "EnemyManager.h"
#include "Project/Player/FPCharacter.h"
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
			myLinkedSpawner->myOnSpawnerFinished.AddDynamic(this, &AEnemySpawner::OnSpawnerFinished);
		break;
	case EEnemySpawnReason::ON_COLLISION:
		if (myLinkedCollider)
			myLinkedCollider->OnActorBeginOverlap.AddDynamic(this, &AEnemySpawner::OnColliderOverlap);
	case EEnemySpawnReason::CUSTOM:
	default:
		break;
	}
}

void AEnemySpawner::EndPlay(const EEndPlayReason::Type aEndPlayReason)
{
	Super::EndPlay(aEndPlayReason);
	if (aEndPlayReason != EEndPlayReason::EndPlayInEditor &&
		aEndPlayReason != EEndPlayReason::Quit &&
		aEndPlayReason != EEndPlayReason::LevelTransition)	
		UMainSingelton::GetEnemyManager().RemoveSpawner(this);
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
	if (!mySpawnedEnemies.Num())
		FinishSpawner();
}

void AEnemySpawner::RemoveEnemy(const AEnemy* anEnemy)
{
	mySpawnedEnemies.Remove(anEnemy);
	if (mySpawnedEnemies.Num() <= 0)
		FinishSpawner();
}

void AEnemySpawner::OnSpawnerFinished(AEnemySpawner* aSpawner)
{
	if (aSpawner == myLinkedSpawner)
		Spawn();
}

void AEnemySpawner::OnColliderOverlap(AActor* aOverlappedActor, AActor* aOtherActor)
{
	if (aOverlappedActor->IsA(AFPCharacter::StaticClass()))
		Spawn();
}

AEnemy* AEnemySpawner::SpawnEnemy(const TSubclassOf<AEnemy>& aClass) const
{
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FTransform trans = GetTransform();
	AEnemy* enemy = Cast<AEnemy>(GetWorld()->SpawnActor(aClass, &trans, params));
	return enemy;
}

void AEnemySpawner::FinishSpawner()
{
	myOnSpawnerFinished.Broadcast(this);
	Destroy();
}
