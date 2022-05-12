#include "EnemyStateAttackHug.h"

#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Enemies/Behaviour/EnemyBehaviour.h"

UEnemyStateAttackHug::UEnemyStateAttackHug()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyStateAttackHug::PerformAttack(AActor* aTarget)
{
	CHECK_RETURN_LOG(!aTarget, "No target");
	auto& self = GetSelf();
	const auto controller = GetSelf().GetController();

	
	UEnemyStateAttackBase::PerformAttack(aTarget);
}
