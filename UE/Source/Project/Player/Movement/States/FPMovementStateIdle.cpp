#include "FPMovementStateIdle.h"

#include "FPMovementStateInAir.h"
#include "FPMovementStateRun.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"

UClass* UFPMovementStateIdle::Update(float aDT)
{	
	const auto& movement = GetCharacterMovement();
	if (movement.IsFalling())
		return UFPMovementStateInAir::StaticClass();
		
	if (movement.GetLastUpdateVelocity().Length() > 0.1f)
		return UFPMovementStateRun::StaticClass();

	return nullptr;
}

TSubclassOf<UFPAnimationStateBase> UFPMovementStateIdle::GetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
}
