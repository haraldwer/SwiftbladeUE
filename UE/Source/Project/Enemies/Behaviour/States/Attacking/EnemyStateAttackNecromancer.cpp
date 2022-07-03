#include "EnemyStateAttackNecromancer.h"

#include "Project/Enemies/Enemy.h"

void UEnemyStateAttackNecromancer::PerformAttack(AActor* aTarget)
{
	for (const auto& it : mySpawnLocations)
	{
		CHECK_CONTINUE(!it);
		auto trans = it->GetComponentTransform();
		const auto actor = GetWorld()->SpawnActor(myEnemy.Get());
		actor->SetActorLocation(trans.GetLocation());
		actor->SetActorRotation(trans.GetRotation());
	}	
	UEnemyStateAttackRangedBase::PerformAttack(aTarget);
}
