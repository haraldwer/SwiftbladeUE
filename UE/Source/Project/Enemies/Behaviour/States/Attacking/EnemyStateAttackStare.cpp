#include "EnemyStateAttackStare.h"

#include "Kismet/GameplayStatics.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Enemies/Behaviour/EnemyBehaviour.h"

UEnemyStateAttackStare::UEnemyStateAttackStare()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyStateAttackStare::Charge(const float aDT)
{
	// Rotate fast
	// Don't move
	
	const auto& behaviour = GetBehaviour();
	behaviour.RotateTowards(behaviour.GetTarget(), myChargeRotationSpeed, aDT);
	
	UEnemyStateAttackBase::Charge(aDT);
}

void UEnemyStateAttackStare::Attack(const float aDT)
{
	// Rotate slowly
	// Stare at player

	const auto& self = GetSelf();
	const auto& behaviour = GetBehaviour();
	const auto target = behaviour.GetTarget();
	behaviour.RotateTowards(target, myAttackRotationSpeed, aDT); 

	if (behaviour.CanDamageTarget())
		PerformAttack(target);
	
	UEnemyStateAttackBase::Attack(aDT);
}

void UEnemyStateAttackStare::Recover(const float aDT)
{
	// Rotate slowly
	
	const auto& behaviour = GetBehaviour();
	behaviour.RotateTowards(behaviour.GetTarget(), myRecoverRotationSpeed, aDT);
	
	UEnemyStateAttackBase::Recover(aDT);
}

void UEnemyStateAttackStare::PerformAttack(AActor* aTarget)
{
	CHECK_RETURN_LOG(!aTarget, "No target");
	auto& self = GetSelf();
	const auto controller = GetSelf().GetController();

	// TODO: Limit player movement
	
	UEnemyStateAttackBase::PerformAttack(aTarget);
}
