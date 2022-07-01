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
	virtual void OnSubStateChanged(EEnemyAttackState aPreviousState) override;

	UPROPERTY(EditDefaultsOnly)
	float myStareRotationSpeed = 30.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myStareValidDot = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float myStareRotInterpStart = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float myStareRotInterpEnd = 50.0f;

	UPROPERTY(EditDefaultsOnly)
	float myStareDistStart = 200.0f;

	UPROPERTY(EditDefaultsOnly)
	float myStareDistEnd= 500.0f;

	UPROPERTY(EditDefaultsOnly)
	float myStareDistEndMul = 0.5f; 
	
	UPROPERTY(EditDefaultsOnly)
	float myStareStartFov = 0.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myStareEndFov = -50.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myFovInterpSpeed = 3.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myStarePullMul = 50.0f;

	UPROPERTY(EditDefaultsOnly)
	float myStareInitialPart = 0.1f;

private:

	float myStareValue = 0.0f;
	float mySmoothFov = 0.0f; 
	FRotator myTargetPreviousRotation;
	
};
