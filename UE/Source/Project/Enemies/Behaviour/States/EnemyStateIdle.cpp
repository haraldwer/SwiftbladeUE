#include "EnemyStateIdle.h"

#include "EnemyStateMove.h"
#include "Project/Enemies/Behaviour/EnemyBehaviour.h"

UEnemyStateIdle::UEnemyStateIdle()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyStateIdle::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemyStateIdle::Update(const float aDT)
{
	Super::Update(aDT);
	
	const float time = GetWorld()->GetTimeSeconds();
	if (time - myLastTargetCheckTime > 0.2f)
	{
		myLastTargetCheckTime = time;
		auto& behaviour = GetBehaviour();
		if (AActor* target = behaviour.FindTarget())
		{
			behaviour.SetTarget(target);
			SetState(UEnemyStateMove::StaticClass());
		}
	}
}
