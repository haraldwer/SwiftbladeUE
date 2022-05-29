#pragma once

#include "CoreMinimal.h"
#include "EnemyStateAttackCharge.h"
#include "EnemyStateAttackHug.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackHug final : public UEnemyStateAttackBase
{
	GENERATED_BODY()

public:
	UEnemyStateAttackHug();

protected:
	virtual void Charge(const float aDT) override;
	virtual void Attack(const float aDT) override;
	virtual void Recover(const float aDT) override;
	virtual void PerformAttack(AActor* aTarget) override;
	virtual void OnSubStateChanged(EEnemyAttackState aPreviousState) override;

	UPROPERTY(EditDefaultsOnly)
	float myRecoverRotationSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float myAttackForwardOffset = -50.0f;

};
