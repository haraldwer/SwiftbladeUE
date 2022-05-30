#include "EnemyStateAttackHug.h"

#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Enemies/Behaviour/EnemyBehaviour.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Combat/FPCombat.h"
#include "Project/Player/Combat/States/FPCombatStateIdle.h"
#include "Project/Player/Combat/States/EnemyAttacks/FPCombatStateHug.h"

UEnemyStateAttackHug::UEnemyStateAttackHug()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyStateAttackHug::Charge(const float aDT)
{
	// TODO: Store initial rotation
	
	// Skip this state
	SetSubState(EEnemyAttackState::ATTACK);
}

void UEnemyStateAttackHug::Attack(const float aDT)
{
	const auto& behaviour = GetBehaviour();
	const auto target = behaviour.GetTarget();
	if (!target)
	{
		SetSubState(EEnemyAttackState::RECOVER);
		return;
	}
	
	// Attach self to player, use initial rotation
	auto& self = GetSelf();
	self.SetActorRotation(target->GetActorRotation().Quaternion() * myAttackRotation.Quaternion());
	self.SetActorLocation(target->GetActorLocation() + self.GetActorForwardVector() * myAttackForwardOffset + self.GetActorUpVector() * myAttackUpOffset);
	self.SetActorEnableCollision(false);

	// Disable player movement
	// Player has to break loose
	// Unique movement state
	const auto character = Cast<AFPCharacter>(target);
	CHECK_RETURN_LOG(!character, "Tried to hug non-player");
	const auto combat = character->GetCombat();
	CHECK_RETURN_LOG(!combat, "Combat nullptr");
	auto state = combat->GetState<UFPCombatStateHug>();
	CHECK_RETURN_LOG(!state, "State nullptr");
	combat->SetState(state);

	// When player has broken loose
	if (state->HasReachedTarget())
	{
		SetSubState(EEnemyAttackState::RECOVER);
		return;
	}

	Super::Attack(aDT);
}

void UEnemyStateAttackHug::Recover(const float aDT)
{
	// Dont do anything
	Super::Recover(aDT);
}

void UEnemyStateAttackHug::PerformAttack(AActor* aTarget)
{
	CHECK_RETURN_LOG(!aTarget, "No target");
	auto& self = GetSelf();
	const auto controller = GetSelf().GetController();
	UGameplayStatics::ApplyDamage(aTarget, 1.0f, controller, &self, UDamageType::StaticClass());	 
	Super::PerformAttack(aTarget);
}

void UEnemyStateAttackHug::OnSubStateChanged(EEnemyAttackState aPreviousState)
{
	if (aPreviousState == EEnemyAttackState::ATTACK &&
		GetSubState() != EEnemyAttackState::ATTACK)
	{
		const auto& behaviour = GetBehaviour();
		const auto target = behaviour.GetTarget();
		const auto character = Cast<AFPCharacter>(target);
		CHECK_RETURN_LOG(!character, "Tried to hug non-player");
		const auto combat = character->GetCombat();
		CHECK_RETURN_LOG(!combat, "Combat nullptr");
		const auto state = combat->GetState<UFPCombatStateHug>();
		CHECK_RETURN_LOG(!state, "State nullptr");
		if (!state->HasReachedTarget())
			PerformAttack(target);
		combat->SetState(combat->GetState<UFPCombatStateIdle>());
		
		auto& self = GetSelf();
		self.SetActorEnableCollision(true); 
	}
}
