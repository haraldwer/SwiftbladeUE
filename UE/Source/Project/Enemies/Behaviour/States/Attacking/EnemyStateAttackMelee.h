#pragma once

#include "CoreMinimal.h"
#include "EnemyStateAttackBase.h"
#include "EnemyStateAttackMelee.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackMelee final : public UEnemyStateAttackBase
{
	GENERATED_BODY()

public:
	UEnemyStateAttackMelee();

protected:
	void Charge(const float aDT) override;
	void Attack(const float aDT) override;
	void Recover(const float aDT) override;
	void PerformAttack(AActor* aTarget) override;

	UPROPERTY(EditDefaultsOnly)
	float myChargeRotationSpeed = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float myRecoverRotationSpeed = 0.5f;

private:
	bool myAttackFirstFrame = false;
};
