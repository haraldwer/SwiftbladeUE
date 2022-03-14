#include "FPMovementStateRun.h"

#include "FPMovementStateIdle.h"
#include "FPMovementStateInAir.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/FPAnimator.h"

UClass* UFPMovementStateRun::Update(float aDT)
{
	auto& animator = GetAnimator();
	animator.SetState(UFPAnimator::State::RUNNING);
	
	const auto& movement = GetCharacterMovement();
	if (movement.IsFalling())
		return UFPMovementStateInAir::StaticClass();
	
	if (movement.GetLastUpdateVelocity().Length() < 0.1f)
		return UFPMovementStateIdle::StaticClass();
	
	return nullptr;
}
