#include "FPMovementStateSlide.h"

#include "FPMovementStateCrouch.h"
#include "FPMovementStateIdle.h"
#include "FPMovementStateInAir.h"
#include "FPMovementStateRun.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/FPCamera.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Animation/States/FPAnimationStateSlide.h"

UClass* UFPMovementStateSlide::Update(float aDT)
{
	auto& movement = GetCharacterMovement();
	if (movement.IsFalling())
		return UFPMovementStateInAir::StaticClass();
	
	if (movement.GetLastUpdateVelocity().Length() < 0.1f)
		return UFPMovementStateIdle::StaticClass();

	if(GetCurrentTime() - mySlideTimeStamp > mySlideDuration)
		return UFPMovementStateRun::StaticClass();

	const float movementDot = FVector::DotProduct(movement.GetLastInputVector(), movement.Velocity);
	if(movementDot < mySlideMinDot)
		return UFPMovementStateRun::StaticClass();
		
	movement.Velocity =
		mySlideDirection *
		movement.MaxWalkSpeed *
		mySlideSpeedMul;

	GetFPCamera().SetTargetHeight(myCamHeightMul);
	
	return nullptr;
}

void UFPMovementStateSlide::Enter()
{
	Super::Enter();
	mySlideTimeStamp = GetCurrentTime();
	auto& character = GetCharacter();
	character.Crouch();
	const auto& movement = GetCharacterMovement();
	mySlideDirection = movement.GetLastUpdateVelocity();
	mySlideDirection.Z = 0;
	mySlideDirection.Normalize();
}

void UFPMovementStateSlide::Exit()
{
	Super::Exit();
	auto& character = GetCharacter();
	character.UnCrouch();
}

TSubclassOf<UFPAnimationStateBase> UFPMovementStateSlide::GetAnimation() const
{
	return UFPAnimationStateSlide::StaticClass();
}
