#include "FPMovementStateStrike.h"

#include "FPMovementStateInAir.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/States/FPAnimationStateDash.h"

UClass* UFPMovementStateStrike::Update(float aDT)
{
	// Exit instantly
	return UFPMovementStateInAir::StaticClass();
}

void UFPMovementStateStrike::Enter()
{
	if (myTarget.Size() < SMALL_NUMBER)
		return; 
	
	const FVector diff = myTarget - GetActorTransform().GetLocation();
	
	auto& movement = GetCharacterMovement();
	//movement.Velocity += diff.GetSafeNormal() * myStrikeVelocity;
}

void UFPMovementStateStrike::Exit()
{
	myTarget = FVector::ZeroVector;
}
