#include "EnemyStateAttackNecromancer.h"

#include "Project/Enemies/Enemy.h"
#include "Project/Enemies/Behaviour/States/EnemyStateMove.h"

void UEnemyStateAttackNecromancer::PerformAttack(AActor* aTarget)
{
	for (const auto& it : mySpawnLocations)
	{
		CHECK_CONTINUE_LOG(!it, "Invalid spawn location");
		const auto bp = myEnemy.Get();
		CHECK_CONTINUE_LOG(!bp, "Invalid bp");
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		auto trans = it->GetComponentTransform();
		trans.SetScale3D(FVector::OneVector);
		const auto actor = GetWorld()->SpawnActor(bp, &trans, params);
		CHECK_CONTINUE_LOG(!actor, "Failed to spawn actor");
	}	
	UEnemyStateAttackRangedBase::PerformAttack(aTarget);
}

void UEnemyStateAttackNecromancer::Recover(const float aDT)
{
	SetState(UEnemyStateMove::StaticClass());
}
