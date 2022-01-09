#include "EnemyStateAttackCharge.h"

#include "Project/Enemies/EnemyBehaviour.h"

UEnemyStateAttackCharge::UEnemyStateAttackCharge()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyStateAttackCharge::Charge(const float aDT)
{
	// Rotate fast
	// Don't move
	
	const auto& behaviour = GetBehaviour();
	behaviour.RotateTowards(behaviour.GetTarget(), myChargeRotationSpeed, aDT);
	
	UEnemyStateAttackBase::Charge(aDT);
}

void UEnemyStateAttackCharge::Attack(const float aDT)
{
	// Don't rotate
	// Move fast

	
	
	const auto& behaviour = GetBehaviour();
	behaviour.MoveTowards(behaviour.GetTarget(), myAttackMovementSpeed, myAttackMovementForwardWeight, aDT);
	
	UEnemyStateAttackBase::Attack(aDT);
}

void UEnemyStateAttackCharge::Recover(const float aDT)
{
	// Rotate slowly
	
	const auto& behaviour = GetBehaviour();
	behaviour.RotateTowards(behaviour.GetTarget(), myRecoverRotationSpeed, aDT);
	
	UEnemyStateAttackBase::Recover(aDT);
}
