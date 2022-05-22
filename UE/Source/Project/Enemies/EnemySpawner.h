#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemySpawnerFinished, AEnemySpawner*, self);

UENUM(BlueprintType)
enum class EEnemySpawnReason : uint8
{
	BEGIN_PLAY,
	ENTER_LEVEL,
	SPAWN_SEQUENCE,
	ON_COLLISION,
	CUSTOM
};

UCLASS()
class PROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:
	
	AEnemySpawner();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable)
	void Spawn();
	void RemoveEnemy(const AEnemy* anEnemy);
	
protected:
	
	UPROPERTY(EditAnywhere)
	EEnemySpawnReason mySpawnReason;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AEnemy>> myEnemiesToSpawn;

	UPROPERTY(EditAnywhere)
	AActor* myLinkedCollider;

	UPROPERTY(EditAnywhere)
	AEnemySpawner* myLinkedSpawner;

	UPROPERTY(BlueprintReadOnly)
	bool myHasSpawned = false;

	UPROPERTY(BlueprintAssignable)
	FEnemySpawnerFinished myOnSpawnerFinished;
	
private:

	UFUNCTION()
	void OnSpawnerFinished(AEnemySpawner* aSpawner);

	UFUNCTION()
	void OnColliderOverlap(AActor* aOverlappedActor, AActor* aOtherActor);
	
	AEnemy* SpawnEnemy(const TSubclassOf<AEnemy>& aClass) const;
	void FinishSpawner();

	UPROPERTY()
	TSet<AEnemy*> mySpawnedEnemies;
};
