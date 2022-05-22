#include "FPCombatStateBase.h"

#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateBase.h"
#include "Project/Player/Combat/FPCombat.h"

void UFPCombatStateBase::Exit()
{
	auto& animator = GetAnimator();
	animator.SetState(animator.GetState(GetResetAnimation()));
}

ASword* UFPCombatStateBase::GetSword() const
{
	return GetCombat().GetSword();
}
