#include "FPCombatStateBlock.h"

#include "FPCombatStateIdle.h"
#include "FPCombatStateNoSword.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Gameplay/Projectile.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateBlock.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"
#include "Project/Player/Combat/FPCombat.h"

void UFPCombatStateBlock::Init()
{
	Super::Init();

	myBlockTimestamp = 0.0f;
}

UClass* UFPCombatStateBlock::Update(float aDT)
{	
	const auto sword = GetSword();
	CHECK_RETURN(!sword, UFPCombatStateNoSword::StaticClass());
	const float timeDiff = GetTime() - myBlockTimestamp;
	CHECK_RETURN(timeDiff > myBlockDuration, UFPCombatStateIdle::StaticClass());

	// Block
	const auto& character = GetCharacter();
	const auto collider = character.GetInteractionCollider();
	CHECK_RETURN_LOG(!collider, "No interaction collider", UFPCombatStateIdle::StaticClass());
	auto overlaps = collider->GetOverlapInfos();
	for (auto& overlap : overlaps)
	{
		const auto actor = overlap.OverlapInfo.GetActor();
		if (const auto projectile = Cast<AProjectile>(actor))
		{
			if (const auto state = GetAnimator().GetState<UFPAnimationStateBlock>())
			{
				state->OnBlock();
				const auto hitTrans = sword->GetHitTransform(projectile);
				sword->CreateHitEffect(hitTrans);
			}
			projectile->Destroy();
		}
	}
	
	return nullptr;
}

UClass* UFPCombatStateBlock::Input(const EFPCombatInput anAction)
{
	CHECK_RETURN(!GetCombat().HasSword(), nullptr);
	CHECK_RETURN(anAction != EFPCombatInput::BLOCK, nullptr);

	// Check cooldown
	const float timeDiff = GetTime() - myBlockTimestamp;
	CHECK_RETURN(timeDiff < myBlockCooldown, nullptr);
	return StaticClass();
}

void UFPCombatStateBlock::Enter()
{
	myBlockTimestamp = GetTime();
}

bool UFPCombatStateBlock::TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator, AActor* aDamageCauser)
{
	// Exceptions to dealt damage here!
	if (aDamageCauser && aDamageCauser->IsA(AEnemy::StaticClass()))
		return true;

	// Block
	if (const auto state = GetAnimator().GetState<UFPAnimationStateBlock>())
	{
		state->OnBlock();
		if (const auto sword = GetSword())
		{
			const auto hitTrans = sword->GetHitTransform(aDamageCauser);
			sword->CreateHitEffect(hitTrans);
		}
	}
	
	return false; 
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateBlock::GetAnimation() const
{
	return UFPAnimationStateBlock::StaticClass();
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateBlock::GetResetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
}
