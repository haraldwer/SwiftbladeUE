#include "EnemyStateAttackBase.h"

#include "Project/Enemies/Behaviour/States/EnemyStateMove.h"
#include "Project/Enemies/Behaviour/EnemyBehaviour.h"
#include "Project/Enemies/Behaviour/States/EnemyStateIdle.h"

void UEnemyStateAttackBase::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemyStateAttackBase::Enter(const UEnemyBaseState* aFromState)
{
	Super::Enter(aFromState);
	SetSubState(EEnemyAttackState::CHARGE);
}

void UEnemyStateAttackBase::Update(const float aDT)
{
	Super::Update(aDT);

	auto& behaviour = GetBehaviour();
	if (!behaviour.CheckTargetVisibility() && myCanAbortAttack)
	{
		behaviour.SetTarget(nullptr);
		SetState(UEnemyStateIdle::StaticClass());
		return;
	}
	
	switch (mySubState)
	{
	case EEnemyAttackState::CHARGE:  Charge(aDT); break;
	case EEnemyAttackState::ATTACK:  Attack(aDT); break;
	case EEnemyAttackState::RECOVER: Recover(aDT); break;
	default:
		SetState(UEnemyStateMove::StaticClass());
	}
}

void UEnemyStateAttackBase::SetSubState(const EEnemyAttackState aSubState)
{
	const auto previousState = mySubState;
	mySubState = aSubState;
	myEnterStateTime = GetWorld()->GetTimeSeconds();
	OnSubStateChanged(previousState);
	myOnAttackStateChangedEvent.Broadcast(aSubState);
}

bool UEnemyStateAttackBase::ShouldChangeState(const float aStateTime) const
{
	if (aStateTime < 0.0f)
		return false; 
	const float currentTime = GetWorld()->GetTimeSeconds();
	return currentTime - myEnterStateTime > aStateTime;
}

void UEnemyStateAttackBase::Charge(const float aDT)
{
	// override with custom code
	
	if (ShouldChangeState(myChargeTime))
		SetSubState(EEnemyAttackState::ATTACK);

	// Cancel attack and return to movement state
	const auto& behaviour = GetBehaviour();
	if (!behaviour.CanAttackTarget())
		SetState(UEnemyStateMove::StaticClass());
}

void UEnemyStateAttackBase::Attack(const float aDT)
{
	// override with custom code

	if (ShouldChangeState(myAttackTime))
		SetSubState(EEnemyAttackState::RECOVER);
}

void UEnemyStateAttackBase::Recover(const float aDT)
{
	// override with custom code
	
	if (ShouldChangeState(myRecoverTime))
		SetState(UEnemyStateMove::StaticClass());
}

void UEnemyStateAttackBase::PerformAttack(AActor* aTarget)
{
	CHECK_RETURN_LOG(!aTarget, "No target");
	myOnPerformAttackEvent.Broadcast(aTarget);
}
