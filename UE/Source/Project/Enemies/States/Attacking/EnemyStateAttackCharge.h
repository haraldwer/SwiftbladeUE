#pragma once

#include "CoreMinimal.h"
#include "EnemyStateAttackBase.h"
#include "EnemyStateAttackCharge.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackCharge final : public UEnemyStateAttackBase
{
	GENERATED_BODY()

public:
	UEnemyStateAttackCharge();

protected:
	void Charge(const float aDT) override;
	void Attack(const float aDT) override;
	void Recover(const float aDT) override;

	UPROPERTY(EditDefaultsOnly)
	float myChargeRotationSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float myAttackMovementSpeed = 1000.0f;
	UPROPERTY(EditDefaultsOnly)
	float myAttackMovementForwardWeight = 0.9f;

	UPROPERTY(EditDefaultsOnly)
	float myRecoverRotationSpeed = 1.0f;
};
