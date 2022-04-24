#include "FPMovementStateRun.h"

#include "FPMovementStateIdle.h"
#include "FPMovementStateInAir.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/States/FPAnimationStateRunning.h"

UClass* UFPMovementStateRun::Update(float aDT)
{
	const auto& movement = GetCharacterMovement();
	if (movement.IsFalling())
		return UFPMovementStateInAir::StaticClass();
	
	if (movement.GetLastUpdateVelocity().Length() < 0.1f)
		return UFPMovementStateIdle::StaticClass();
	
	return nullptr;
}

TSubclassOf<UFPAnimationStateBase> UFPMovementStateRun::GetAnimation() const
{
	return UFPAnimationStateRunning::StaticClass();
}
