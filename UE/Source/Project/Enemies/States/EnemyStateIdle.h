#pragma once

#include "CoreMinimal.h"
#include "EnemyBaseState.h"
#include "EnemyStateIdle.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateIdle : public UEnemyBaseState
{
	GENERATED_BODY()

public:
	UEnemyStateIdle();

protected:
	virtual void BeginPlay() override;
	virtual void Update(const float aDT) override;

private:
	float myLastTargetCheckTime = 0.0f;
};
