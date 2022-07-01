#include "EnemyStateAttackCharge.h"

#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Enemies/Behaviour/EnemyBehaviour.h"

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
	const auto& behaviour = GetBehaviour();
	const auto target = behaviour.GetTarget();
	if (!target)
	{
		SetSubState(EEnemyAttackState::RECOVER);
		return;
	}

	// Move fast
	behaviour.MoveTowards(target, myAttackMovementSpeed, myAttackMovementForwardWeight, aDT);

	// Deal damage if close
	if (behaviour.CanDamageTarget())
		PerformAttack(target);
	
	UEnemyStateAttackBase::Attack(aDT);
}

void UEnemyStateAttackCharge::Recover(const float aDT)
{
	// Rotate slowly
	const auto& behaviour = GetBehaviour();
	behaviour.RotateTowards(behaviour.GetTarget(), myRecoverRotationSpeed, aDT);
	UEnemyStateAttackBase::Recover(aDT);
}

void UEnemyStateAttackCharge::PerformAttack(AActor* aTarget)
{
	CHECK_RETURN_LOG(!aTarget, "No target");
	auto& self = GetSelf();
	const auto controller = GetSelf().GetController();
	UGameplayStatics::ApplyDamage(aTarget, 1.0f, controller, &self, UDamageType::StaticClass());	
	UEnemyStateAttackBase::PerformAttack(aTarget);
}
