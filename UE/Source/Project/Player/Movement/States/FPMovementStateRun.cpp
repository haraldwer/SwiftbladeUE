#include "FPMovementStateRun.h"

#include "FPMovementStateIdle.h"
#include "FPMovementStateInAir.h"
#include "GameFramework/CharacterMovementComponent.h"

UClass* UFPMovementStateRun::Update(float aDT)
{
	const auto& movement = GetCharacterMovement();
	if (movement.IsFalling())
		return UFPMovementStateInAir::StaticClass();
	
	if (movement.GetLastUpdateVelocity().Length() < 0.1f)
		return UFPMovementStateIdle::StaticClass();
	
	return nullptr;
}
