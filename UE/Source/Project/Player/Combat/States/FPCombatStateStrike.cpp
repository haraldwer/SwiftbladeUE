#include "FPCombatStateStrike.h"

#include "FPCombatStateIdle.h"
#include "FPCombatStateNoSword.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Gameplay/Checkpoint.h"
#include "Project/Player/FPController.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"
#include "Project/Player/Animation/States/FPAnimationStateStrike.h"
#include "Project/Player/Combat/FPCombat.h"

UClass* UFPCombatStateStrike::Update(float aDT)
{
	const float timeDiff = GetTime() - myStrikeTimestamp;
	if (timeDiff > myStrikeDuration)
		return UFPCombatStateIdle::StaticClass();
	if (timeDiff > myStrikeDamageDuration)
		return nullptr;

	const auto sword = GetSword();
	CHECK_RETURN(!sword, UFPCombatStateNoSword::StaticClass());

	// Checkpoints
	for (const auto& c : sword->GetOverlaps(ACheckpoint::StaticClass()))
	{
		const auto checkpoint = Cast<ACheckpoint>(c);
		CHECK_CONTINUE_LOG(!checkpoint, "Not a checkpoint")
		if (GetController().TrySetCheckpoint(checkpoint))
		{
			sword->CreateHitEffect(checkpoint);
			break;
		}
	}

	// Enemies
	// Only hit one enemy per strike
	CHECK_RETURN(myHasHit, nullptr);
	for (const auto& e : sword->GetOverlaps(AEnemy::StaticClass()))
	{
		const auto enemy = Cast<AEnemy>(e);
		CHECK_CONTINUE_LOG(!sword, "Sword nullptr");
		UGameplayStatics::ApplyDamage(
			enemy,
			1.0f,
			&GetController(),
			sword,
			UDamageType::StaticClass());
		sword->CreateHitEffect(enemy); 
		myHasHit = true; 
		break;
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
