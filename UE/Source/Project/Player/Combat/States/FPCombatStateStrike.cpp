#include "FPCombatStateStrike.h"

#include "FPCombatStateIdle.h"
#include "FPCombatStateNoSword.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Gameplay/Checkpoint.h"
#include "Project/Gameplay/Breakable/Breakable.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"
#include "Project/Player/Animation/States/FPAnimationStateStrike.h"
#include "Project/Player/Combat/FPCombat.h"
#include "Project/Player/Movement/FPMovement.h"
#include "Project/Player/Movement/States/FPMovementStateStrike.h"

UClass* UFPCombatStateStrike::Update(float aDT)
{
	const float timeDiff = GetTime() - myStrikeTimestamp;
	if (timeDiff > myStrikeDuration)
		return UFPCombatStateIdle::StaticClass();
	if (timeDiff > myStrikeDamageDuration)
		return nullptr;

	if (myHasHit)
		return nullptr;

	const auto sword = GetSword();
	CHECK_RETURN_LOG(!sword, "No sword", UFPCombatStateNoSword::StaticClass()); 

	const auto& character = GetCharacter();
	const auto collider = character.GetInteractionCollider();
	CHECK_RETURN_LOG(!collider, "No interaction collider", UFPCombatStateIdle::StaticClass());

	auto overlaps = collider->GetOverlapInfos();
	for (auto& overlap : overlaps)
	{
		const auto component = overlap.OverlapInfo.Component.Get();
		CHECK_CONTINUE(!component);
		AActor* owner = component->GetOwner();
		CHECK_CONTINUE(!owner);
		CHECK_CONTINUE(owner == &character);
		
		// Checkpoints
		if (const auto checkpoint = Cast<ACheckpoint>(owner))
		{
			if (GetController().TrySetCheckpoint(checkpoint))
			{
				const auto hitTrans = sword->GetHitTransform(checkpoint);
				sword->CreateHitEffect(hitTrans);
			}
			myHasHit = true;
			break;
		}

		// Enemies
		if (const auto enemy = Cast<AEnemy>(owner))
		{
			// Enter strike movement state
			auto& movement = GetMovement();
			if (const auto strikeState = movement.GetState<UFPMovementStateStrike>())
			{
				strikeState->SetTarget(enemy->GetActorLocation());
				movement.SetState<UFPMovementStateStrike>();
			}
			
			const FTransform hitTrans = sword->GetHitTransform(enemy);
			sword->CreateHitEffect(hitTrans);
			
			if (const auto enemyCollider = enemy->GetCollider())
				enemyCollider->AddImpulseAtLocation((enemy->GetActorLocation() - character.GetActorLocation()).GetSafeNormal() * myStrikeImpulse, hitTrans.GetLocation());
			
			UGameplayStatics::ApplyDamage(
				enemy,
				1.0f,
				&GetController(),
				sword,
				UDamageType::StaticClass());
			
			myHasHit = true;
			LOG("Apply damage");
			
			break;
		}
		
		// Breakable stuff
		if (const auto breakable = Cast<UBreakable>(owner->GetComponentByClass(UBreakable::StaticClass())))
		{
			const auto hitTrans = sword->GetHitTransform(owner);
			sword->CreateHitEffect(hitTrans);
			const auto position = sword->GetActorLocation();
			const auto normal = sword->GetActorRightVector(); 
			breakable->Break(position, normal);

			// Enter strike movement state
			// TODO: Maybe make optional?
			auto& movement = GetMovement();
			if (const auto strikeState = movement.GetState<UFPMovementStateStrike>())
			{
				strikeState->SetTarget(owner->GetActorLocation());
				movement.SetState<UFPMovementStateStrike>();
			}
			
			break; 
		}
	}
	
	return nullptr;
}

UClass* UFPCombatStateStrike::Input(const EFPCombatInput anAction)
{
	CHECK_RETURN(!GetCombat().HasSword(), nullptr);
	CHECK_RETURN(anAction != EFPCombatInput::STRIKE, nullptr);

	// Check cooldown
	const float timeDiff = GetTime() - myStrikeTimestamp;
	CHECK_RETURN(timeDiff < myStrikeCooldown, nullptr);
	return StaticClass();
}

void UFPCombatStateStrike::Enter()
{
	myHasHit = false;
	myStrikeTimestamp = GetTime();
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateStrike::GetAnimation() const
{
	return UFPAnimationStateStrike::StaticClass();
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateStrike::GetResetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
}
