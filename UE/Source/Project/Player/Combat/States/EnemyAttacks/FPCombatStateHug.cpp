#include "FPCombatStateHug.h"

#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"
#include "Project/Player/Animation/States/EnemyAttacks/FPAnimationStateHug.h"
#include "Project/Player/Combat/FPCombat.h"
#include "Project/Player/Movement/FPMovement.h"
#include "Project/Player/Movement/States/FPMovementStateInAir.h"
#include "Project/Player/Movement/States/EnemyAttacks/FPMovementStateHug.h"

void UFPCombatStateHug::Enter()
{
	auto& movement = GetMovement();
	movement.SetState<UFPMovementStateHug>();

	myValue = 0.0f;
	myVelocity = 0.0f;
}

void UFPCombatStateHug::Exit()
{
	auto& movement = GetMovement();
	movement.SetState<UFPMovementStateInAir>();
}

UClass* UFPCombatStateHug::Update(float aDT)
{
	myValue += myVelocity; 
	myVelocity = FMath::FInterpTo(myVelocity, 0.0f, aDT, myFriction);
	LOG("Value: " + FString::SanitizeFloat(myValue) + " Velocity: " + FString::SanitizeFloat(myVelocity));
	
	return StaticClass();
}

UClass* UFPCombatStateHug::Input(EFPCombatInput anAction)
{
	if (IsCurrentState())
	{
		if (anAction == EFPCombatInput::STRIKE)
		{
			myVelocity += myAcceleration;
			if (const auto state = GetAnimator().GetState<UFPAnimationStateHug>())
				state->Wiggle(FMath::Max(myValue / myTargetValue, 0.1f));
		}
		return StaticClass();
	}
	return nullptr; 
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateHug::GetAnimation() const
{
	return UFPAnimationStateHug::StaticClass();
}

TSubclassOf<UFPAnimationStateBase> UFPCombatStateHug::GetResetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
}
