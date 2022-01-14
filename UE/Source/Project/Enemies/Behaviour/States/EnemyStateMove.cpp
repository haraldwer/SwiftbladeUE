#include "EnemyStateMove.h"

#include "Attacking/EnemyStateAttackBase.h"
#include "EnemyStateIdle.h"
#include "Project/Enemies/Behaviour/EnemyBehaviour.h"

UEnemyStateMove::UEnemyStateMove()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyStateMove::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemyStateMove::Update(const float aDT)
{
	Super::Update(aDT);

	auto& self = GetSelf();
	auto& behaviour = GetBehaviour();
	
	if (!behaviour.CheckTargetVisibility())
	{
		behaviour.SetTarget(nullptr);
		SetState(UEnemyStateIdle::StaticClass());
		return;
	}
	
	// Apply movement
	AActor* target = behaviour.GetTarget();
	behaviour.MoveTowards(target, myMovementSpeed, myForwardWeight, aDT);
	behaviour.RotateTowards(target, myRotationSpeed, aDT);
	
	if (behaviour.CanAttackTarget())
		SetState(UEnemyStateAttackBase::StaticClass());
}
