#pragma once

#include "CoreMinimal.h"
#include "EnemyStateAttackCharge.h"
#include "EnemyStateAttackHug.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackHug final : public UEnemyStateAttackCharge
{
	GENERATED_BODY()

public:
	UEnemyStateAttackHug();

protected:
	virtual void PerformAttack(AActor* aTarget) override;

};
