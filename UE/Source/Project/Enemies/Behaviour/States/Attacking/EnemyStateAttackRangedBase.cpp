#include "EnemyStateAttackRangedBase.h"

#include "Project/Enemies/EnemyBehaviour.h"

UEnemyStateAttackRangedBase::UEnemyStateAttackRangedBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyStateAttackRangedBase::Charge(const float aDT)
{
	// Rotate slowly
	// Don't move
	
	myAttackFirstFrame = true;
	
	const auto& behaviour = GetBehaviour();
	behaviour.RotateTowards(behaviour.GetTarget(), myChargeRotationSpeed, aDT);
	
	UEnemyStateAttackBase::Charge(aDT);
}

void UEnemyStateAttackRangedBase::Attack(const float aDT)
{
	if (myAttackFirstFrame || myPerformAttackEveryFrame)
	{
		myAttackFirstFrame = false;
		const auto& behaviour = GetBehaviour();
		const auto target = behaviour.GetTarget();
		PerformAttack(target);
	}
	
	UEnemyStateAttackBase::Attack(aDT);
}

void UEnemyStateAttackRangedBase::Recover(const float aDT)
{
	// Rotate slowly
	// Don't move

	const auto& behaviour = GetBehaviour();
	behaviour.RotateTowards(behaviour.GetTarget(), myRecoverRotationSpeed, aDT);
	
	UEnemyStateAttackBase::Recover(aDT);
}

void UEnemyStateAttackRangedBase::PerformAttack(AActor* aTarget)
{
	// override 
	
	UEnemyStateAttackBase::PerformAttack(aTarget);
}

