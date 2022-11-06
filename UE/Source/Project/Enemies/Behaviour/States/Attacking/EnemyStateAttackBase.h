﻿#pragma once

#include "CoreMinimal.h"
#include "Project/Enemies/Behaviour/States/EnemyBaseState.h"
#include "EnemyStateAttackBase.generated.h"

UENUM(BlueprintType)
enum class EEnemyAttackState : uint8
{
	CHARGE,
	ATTACK,
	RECOVER,
	COUNT
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyOnPerformAttack, AActor*, aTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyOnAttackStateChanged, EEnemyAttackState, aState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackBase : public UEnemyBaseState
{
	GENERATED_BODY()
	
public:
	
	virtual void BeginPlay() override;
	virtual void Enter(const UEnemyBaseState* aFromState) override;
	virtual void Update(const float aDT) override;

protected:

	virtual void Charge(const float aDT);
	virtual void Attack(const float aDT);
	virtual void Recover(const float aDT);
	virtual void PerformAttack(AActor* aTarget);
	virtual void OnSubStateChanged(EEnemyAttackState aPreviousState) {}
	
	void SetSubState(EEnemyAttackState aSubState);
	EEnemyAttackState GetSubState() const { return mySubState; }
	
	UPROPERTY(BlueprintReadWrite)
	FEnemyOnPerformAttack myOnPerformAttackEvent;
	
	UPROPERTY(BlueprintReadWrite)
	FEnemyOnAttackStateChanged myOnAttackStateChangedEvent;
	
	UPROPERTY(EditDefaultsOnly)
	float myChargeTime = -1.0f;
	UPROPERTY(EditDefaultsOnly)
	float myAttackTime = -1.0f;
	UPROPERTY(EditDefaultsOnly)
	float myRecoverTime = -1.0f;
	
	UPROPERTY(EditDefaultsOnly)
	bool myCanAbortAttack = true;

private:
	
	bool ShouldChangeState(const float aStateTime) const;
	
	EEnemyAttackState mySubState = EEnemyAttackState::CHARGE;
	float myEnterStateTime = 0.0f;
};
