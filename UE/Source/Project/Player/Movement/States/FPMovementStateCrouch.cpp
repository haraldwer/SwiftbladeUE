#include "FPMovementStateCrouch.h"

#include "FPMovementStateInAir.h"
#include "FPMovementStateRun.h"
#include "FPMovementStateSlide.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/FPCamera.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Animation/States/FPAnimationStateIdle.h"

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

UClass* UFPMovementStateCrouch::Input(const EFPMovementInputAction anAction, const float aValue)
{
	Super::Input(anAction, aValue);

	if (anAction == EFPMovementInputAction::CROUCH_PRESSED)
	{
		myIsHoldingCrouch = true;
	}
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
}

TSubclassOf<UFPAnimationStateBase> UFPMovementStateCrouch::GetAnimation() const
{
	return UFPAnimationStateIdle::StaticClass();
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

	if (HasMagic())
		if (const auto slide = GetState<UFPMovementStateSlide>())
			if (!slide->IsOnCooldown())
				if (movement.GetLastUpdateVelocity().Size2D() > myEnterSlideSpeed)
					return UFPMovementStateSlide::StaticClass();

	return StaticClass();
}
