#include "FPMovementStateStrike.h"

#include "FPMovementStateInAir.h"
#include "GameFramework/CharacterMovementComponent.h"

UClass* UFPMovementStateStrike::Update(float aDT)
{	
	return UFPMovementStateInAir::StaticClass();
}

void UFPMovementStateStrike::Enter()
{
	ResetAbilities();
}

void UFPMovementStateStrike::Exit()
{
	auto& movement = GetCharacterMovement();
	movement.Velocity += movement.GetLastInputVector().GetSafeNormal2D() * myExitVelocity.X;
	if (movement.Velocity.Z < myExitVelocity.Z && !movement.IsWalking())
		movement.Velocity.Z = myExitVelocity.Z;
}
