#pragma once

#include "CoreMinimal.h"
#include "Project/Enemies/States/EnemyBaseState.h"
#include "EnemyStateAttackBase.generated.h"

UENUM(BlueprintType)
enum class EEnemyAttackState : uint8
{
	CHARGE,
	ATTACK,
	RECOVER,
	COUNT
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubStateChanged, EEnemyAttackState, aState);

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
	
	UPROPERTY(EditDefaultsOnly)
	float myChargeTime = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float myAttackTime = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float myRecoverTime = 1.0f;

	// For effects related to attack states
	UPROPERTY(BlueprintAssignable)
	FSubStateChanged myOnAttackStateChanged;
	
private:
	void SetSubState(EEnemyAttackState aSubState);
	bool ShouldChangeState(const float aStateTime) const;
	
	EEnemyAttackState mySubState = EEnemyAttackState::CHARGE;
	float myEnterStateTime = 0.0f;
	
};
