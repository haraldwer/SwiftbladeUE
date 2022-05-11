﻿#include "FPCombatStateBlock.h"

#include "FPCombatStateIdle.h"
#include "FPCombatStateNoSword.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	// Blocked damage
	//for (const auto& c : sword->GetOverlaps(ADamage::StaticClass()))
	//{
	//	if (FMath::RandBool())
	//		if (const auto state = GetAnimator().GetState<UFPAnimationStateBlock>())
	//			state->OnBlock();		
	//}
	
	const float timeDiff = GetTime() - myBlockTimestamp;
	CHECK_RETURN(timeDiff > myBlockDuration, UFPCombatStateIdle::StaticClass());
	return nullptr;
}

UClass* UFPCombatStateBlock::Input(const EFPCombatInput anAction)
{
	CHECK_RETURN(!GetCombat().HasSword(), nullptr);
	CHECK_RETURN(anAction != EFPCombatInput::BLOCK, nullptr);

	// Check cooldown
	const float timeDiff = GetTime() - myBlockTimestamp;	CHECK_RETURN(timeDiff < myBlockCooldown, nullptr);
	return StaticClass();
}

void UFPCombatStateBlock::Enter()
{
	myBlockTimestamp = GetTime();
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateBlock::GetAnimation() const
{
	return UFPAnimationStateBlock::StaticClass();
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateBlock::GetResetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
}