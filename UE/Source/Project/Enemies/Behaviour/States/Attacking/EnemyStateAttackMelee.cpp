#include "EnemyStateAttackMelee.h"

#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Enemies/EnemyBehaviour.h"

UEnemyStateAttackMelee::UEnemyStateAttackMelee()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyStateAttackMelee::Charge(const float aDT)
{
	// Rotate slowly
	// Don't move

	myAttackFirstFrame = false;
	
	const auto& behaviour = GetBehaviour();
	behaviour.RotateTowards(behaviour.GetTarget(), myChargeRotationSpeed, aDT);
	
	UEnemyStateAttackBase::Charge(aDT);
}

void UEnemyStateAttackMelee::Attack(const float aDT)
{
	// Don't move
	// Deal damage if close
	// Only deal damage first frame

	if (myAttackFirstFrame)
	{
		myAttackFirstFrame = false;
		const auto& behaviour = GetBehaviour();
		const auto target = behaviour.GetTarget();
		PerformAttack(target);
	}
	
	UEnemyStateAttackBase::Attack(aDT);
}

void UEnemyStateAttackMelee::Recover(const float aDT)
{
	// Rotate slowly

	const auto& behaviour = GetBehaviour();
	behaviour.RotateTowards(behaviour.GetTarget(), myRecoverRotationSpeed, aDT);
	
	UEnemyStateAttackBase::Recover(aDT);
}

void UEnemyStateAttackMelee::PerformAttack(AActor* aTarget)
{
	CHECK_RETURN_LOG(aTarget, "No target");
	auto& self = GetSelf();
	const auto controller = GetSelf().GetController();
	const auto& behaviour = GetBehaviour();
	if (behaviour.CanDamageTarget())
		UGameplayStatics::ApplyDamage(aTarget, 1.0f, controller, &self, UDamageType::StaticClass());	
	UEnemyStateAttackBase::PerformAttack(aTarget);
}
