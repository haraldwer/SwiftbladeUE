#include "FPCombatStateBlock.h"

#include "FPCombatStateIdle.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/States/FPAnimationStateBlock.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"
#include "Project/Player/Combat/FPCombat.h"

UClass* UFPCombatStateBlock::Update(float aDT)
{	
	LOG("Block");
	const float timeDiff = GetTime() - myBlockTimestamp;
	if (timeDiff > myBlockDuration)
		return UFPCombatStateIdle::StaticClass();

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

TSubclassOf<UFPAnimationStateBase> UFPCombatStateBlock::GetAnimation() const
{
	return UFPAnimationStateBlock::StaticClass();
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateBlock::GetResetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
}
