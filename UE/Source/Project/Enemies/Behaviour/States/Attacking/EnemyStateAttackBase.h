#pragma once

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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyStateAttackBase : public UEnemyBaseState
{
	GENERATED_BODY()

public:
	UEnemyStateAttackBase();
	
protected:
	void BeginPlay() override;
	void Enter(const UEnemyBaseState* aFromState) override;
	void Update(const float aDT) override;

	virtual void Charge(const float aDT);
	virtual void Attack(const float aDT);
	virtual void Recover(const float aDT);

	virtual void PerformAttack(AActor* aTarget);

	// DOES NOT WORK IN COMPONENTS!!
	
	// For effects related to performing attack
	UFUNCTION(BlueprintImplementableEvent)
	void OnPerformAttack(AActor* aTarget);
	
	// For effects related to attack states
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackStateChanged(EEnemyAttackState aState);
	
	UPROPERTY(EditDefaultsOnly)
	float myChargeTime = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float myAttackTime = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float myRecoverTime = 1.0f;

private:
	void SetSubState(EEnemyAttackState aSubState);
	bool ShouldChangeState(const float aStateTime) const;
	
	EEnemyAttackState mySubState = EEnemyAttackState::CHARGE;
	float myEnterStateTime = 0.0f;
	
};
