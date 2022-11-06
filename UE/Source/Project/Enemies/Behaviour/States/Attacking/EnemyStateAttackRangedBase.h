#pragma once

#include "CoreMinimal.h"
#include "EnemyStateAttackBase.h"
#include "EnemyStateAttackRangedBase.generated.h"

class AProjectile; 

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackRangedBase : public UEnemyStateAttackBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	
	virtual void Charge(const float aDT) override;
	virtual void Attack(const float aDT) override;
	virtual void Recover(const float aDT) override;
	virtual void PerformAttack(AActor* aTarget) override;

	virtual void OnDied() override;

	UPROPERTY(EditDefaultsOnly)
	float myChargeRotationSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float myRecoverRotationSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	bool myPerformAttackEveryFrame = false;

	UPROPERTY(EditDefaultsOnly)
	bool myDestroyProjectilesOnDeath = false;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> myProjectileType;
	
private:
	
	UPROPERTY()
	TArray<TWeakObjectPtr<USceneComponent>> mySpawnLocations;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> mySpawnedProjectiles;
	
	bool myAttackFirstFrame = false;
};
