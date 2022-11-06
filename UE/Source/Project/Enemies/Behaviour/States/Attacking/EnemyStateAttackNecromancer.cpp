#include "EnemyStateAttackNecromancer.h"

#include "Project/Enemies/Behaviour/States/EnemyStateMove.h"

void UEnemyStateAttackNecromancer::Recover(const float aDT)
{
	SetState(UEnemyStateMove::StaticClass());
}
