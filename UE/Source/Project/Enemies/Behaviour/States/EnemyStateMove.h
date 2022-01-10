#pragma once

#include "CoreMinimal.h"
#include "EnemyBaseState.h"
#include "EnemyStateMove.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateMove : public UEnemyBaseState
{
	GENERATED_BODY()

public:
	UEnemyStateMove();

protected:
	virtual void BeginPlay() override;
	virtual void Update(float aDT) override;

	UPROPERTY(EditDefaultsOnly)
	float myRotationSpeed = 5.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myMovementSpeed = 500.0f;

	UPROPERTY(EditDefaultsOnly)
	float myForwardWeight = 0.5f;
};
