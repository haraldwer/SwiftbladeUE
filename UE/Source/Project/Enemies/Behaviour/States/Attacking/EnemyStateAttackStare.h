#pragma once

#include "CoreMinimal.h"
#include "EnemyStateAttackBase.h"
#include "EnemyStateAttackStare.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackStare : public UEnemyStateAttackBase
{
	GENERATED_BODY()

public:
	UEnemyStateAttackStare();

protected:
	void Charge(const float aDT) override;
	void Attack(const float aDT) override;
	void Recover(const float aDT) override;
	virtual void PerformAttack(AActor* aTarget) override;

	UPROPERTY(EditDefaultsOnly)
	float myChargeRotationSpeed = 30.0f;

	UPROPERTY(EditDefaultsOnly)
	float myAttackRotationSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float myRecoverRotationSpeed = 1.0f;
};
