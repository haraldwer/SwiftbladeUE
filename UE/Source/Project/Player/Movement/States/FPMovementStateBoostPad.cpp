#include "FPMovementStateBoostPad.h"

#include "FPMovementStateInAir.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Gameplay/BoostPad.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Animation/States/FPAnimationStateInAir.h"

UClass* UFPMovementStateBoostPad::Update(float aDT)
{
	auto& movement = GetCharacterMovement();
	movement.Velocity = myBoostDirection * myBoostVelocity;

	const float timer = GetCurrentTime() - myBoostTimestamp;  
	if (timer > myBoostDuration)
		return UFPMovementStateInAir::StaticClass();
	
	return nullptr;
}

UClass* UFPMovementStateBoostPad::Check()
{
	// Check overlaps
	TArray<AActor*> overlaps;
	GetCharacter().GetOverlappingActors(overlaps, ABoostPad::StaticClass());
	for (const auto& overlap : overlaps)
	{
		myBoostLocation = overlap->GetActorLocation();
		myBoostDirection = overlap->GetActorForwardVector();
		return StaticClass();
	}
	return nullptr;
}

void UFPMovementStateBoostPad::Enter()
{
	myBoostTimestamp = GetCurrentTime();
	auto& movement = GetCharacterMovement();
	GetCharacter().SetActorLocation(myBoostLocation);
	movement.SetMovementMode(MOVE_Falling);
	movement.Velocity = myBoostDirection * myBoostVelocity;
}

void UFPMovementStateBoostPad::Exit()
{
	auto& movement = GetCharacterMovement();
	movement.Velocity = myBoostDirection * movement.MaxWalkSpeed;
}

TSubclassOf<UFPAnimationStateBase> UFPMovementStateBoostPad::GetAnimation() const
{
	return UFPAnimationStateInAir::StaticClass();
}
