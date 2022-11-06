#pragma once

#include "CoreMinimal.h"
#include "EnemyStateAttackRangedBase.h"
#include "Components/ActorComponent.h"
#include "EnemyStateAttackNecromancer.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackNecromancer : public UEnemyStateAttackRangedBase
{
	GENERATED_BODY()

public:
	
	virtual void Recover(const float aDT) override;
	
};
