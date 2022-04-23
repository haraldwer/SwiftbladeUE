#include "FPMovementStateIdle.h"

#include "FPMovementStateInAir.h"
#include "FPMovementStateRun.h"
#include "GameFramework/CharacterMovementComponent.h"

UClass* UFPMovementStateIdle::Update(float aDT)
{	
	const auto& movement = GetCharacterMovement();
	if (movement.IsFalling())
		return UFPMovementStateInAir::StaticClass();
		
	if (movement.GetLastUpdateVelocity().Length() > 0.1f)
		return UFPMovementStateRun::StaticClass();

	return nullptr;
}
