#pragma once

#include "CoreMinimal.h"
#include "EnemyStateAttackBase.h"
#include "Components/ActorComponent.h"
#include "EnemyStateAttackRangedBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackRangedBase : public UEnemyStateAttackBase
{
	GENERATED_BODY()

public:
	UEnemyStateAttackRangedBase();

protected:
	void Charge(const float aDT) override;
	void Attack(const float aDT) override;
	void Recover(const float aDT) override;
	void PerformAttack(AActor* aTarget) override;

	UPROPERTY(EditDefaultsOnly)
	float myChargeRotationSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float myRecoverRotationSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	bool myPerformAttackEveryFrame = false;
	
private:
	bool myAttackFirstFrame = false;
};
