#include "FPMovementStateCrouch.h"

#include "FPMovementStateInAir.h"
#include "FPMovementStateRun.h"
#include "FPMovementStateSlide.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/FPCamera.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Animation/FPAnimator.h"

UClass* UFPMovementStateCrouch::Update(float aDT)
{
	const auto& movement = GetCharacterMovement();
	if (movement.IsFalling())
		return UFPMovementStateInAir::StaticClass();

	auto& fpCamera = GetFPCamera();
	fpCamera.SetTargetHeight(myCamHeightMul);
	
	return nullptr;
}

UClass* UFPMovementStateCrouch::Check()
{
	return CheckCrouch();
}

UClass* UFPMovementStateCrouch::Input(EFPMovementInputAction anAction, float aValue)
{
	Super::Input(anAction, aValue);

	if (anAction == EFPMovementInputAction::CROUCH_PRESSED)
		myIsHoldingCrouch = true;
	if (anAction == EFPMovementInputAction::CROUCH_RELEASED)
	{
		myIsHoldingCrouch = false;
		if (IsCurrentState())
			return UFPMovementStateRun::StaticClass();
	}
	
	return CheckCrouch();
}

UClass* UFPMovementStateCrouch::OnLanded()
{
	return CheckCrouch(true); 
}

void UFPMovementStateCrouch::Enter()
{
	Super::Enter();
	auto& character = GetCharacter();
	character.Crouch();
}

void UFPMovementStateCrouch::Exit()
{
	Super::Exit();
	auto& character = GetCharacter();
	character.UnCrouch();
	auto& animator = GetAnimator();
	animator.MoveLeft(FVector(0, 0, -50));
	animator.MoveRight(FVector(0, 0, -50));
	auto& fpCamera = GetFPCamera();
}

UClass* UFPMovementStateCrouch::CheckCrouch(const bool aLanded) const
{
	if (IsCurrentState())
		return nullptr;
	
	if (!myIsHoldingCrouch)
		return nullptr;
	
	const auto& movement = GetCharacterMovement();
	if (!movement.IsWalking() && !aLanded)
		return nullptr;
	
	const auto& character = GetCharacter();
	if (character.bIsCrouched)
		return nullptr;

	// Slide? 
	if (movement.GetLastUpdateVelocity().Size2D() > movement.MaxWalkSpeedCrouched)
		return UFPMovementStateSlide::StaticClass();
	
	return StaticClass();
}
