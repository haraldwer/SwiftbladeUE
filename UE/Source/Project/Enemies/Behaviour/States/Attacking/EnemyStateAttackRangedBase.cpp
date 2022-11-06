#include "EnemyStateAttackRangedBase.h"

#include "Project/Enemies/Enemy.h"
#include "Project/Enemies/Behaviour/EnemyBehaviour.h"
#include "Project/Gameplay/Projectile.h"

void UEnemyStateAttackRangedBase::BeginPlay()
{
	Super::BeginPlay();

	// Add spawn locations
	for (auto& comp : GetSelf().GetComponents())
		if (const auto sceneComp = Cast<USceneComponent>(comp))
			if (sceneComp->GetName() == "SpawnLocations")
				for (auto& child : sceneComp->GetAttachChildren())
					if (const auto childPtr = child.Get())
						mySpawnLocations.Add(childPtr);
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
	for (const auto& it : mySpawnLocations)
	{
		const auto spawnLoc = it.Get();
		CHECK_CONTINUE_LOG(!spawnLoc, "Invalid spawn location");
		const auto bp = myProjectileType.Get();
		CHECK_CONTINUE_LOG(!bp, "Invalid bp");
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		auto trans = spawnLoc->GetComponentTransform();
		trans.SetScale3D(FVector::OneVector);
		const auto actor = GetWorld()->SpawnActor(bp, &trans, params);
		CHECK_CONTINUE_LOG(!actor, "Failed to spawn actor");
		if (const auto projectile = Cast<AProjectile>(actor))
			projectile->SetInstigator(&GetSelf());
		mySpawnedProjectiles.Add(actor);
	}
	
	UEnemyStateAttackBase::PerformAttack(aTarget);
}

void UEnemyStateAttackRangedBase::OnDied()
{
	Super::OnDied();

	CHECK_RETURN(!myDestroyProjectilesOnDeath);
	for (auto& projectile : mySpawnedProjectiles)
		if (const auto ptr = projectile.Get())
			ptr->Destroy();
}
