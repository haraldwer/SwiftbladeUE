﻿#include "FPMovementStateInAir.h"

#include "FPMovementStateRun.h"
#include "FPMovementStateWall.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/States/FPAnimationStateInAir.h"

void UFPMovementStateInAir::Init()
{
	Super::Init();

	// Prevent super jump on respawn
	myJumpedSinceTouchedGround = true; 
}

UClass* UFPMovementStateInAir::Update(float aDT)
{
	const auto& movement = GetCharacterMovement();
	if (movement.IsWalking())
		return UFPMovementStateRun::StaticClass();
	
	return nullptr;
}

UClass* UFPMovementStateInAir::Check()
{
	const float time = GetCurrentTime();
	const auto& movement = GetCharacterMovement();
	if (movement.IsWalking())
		myCoyoteTimeStamp = time;
	return nullptr;
}

UClass* UFPMovementStateInAir::Input(EFPMovementInputAction anAction, float aValue)
{
	Super::Input(anAction, aValue);

	if (const auto currentState = Cast<UFPMovementStateBase>(GetCurrentState()))
	{
		if (!currentState->CanJump())
		{
			myJumpHeld = false;
			return nullptr;
		}
	}
		
	if (anAction == EFPMovementInputAction::JUMP_PRESSED)
	{
		myJumpHeld = true;
		const float time = GetCurrentTime();
		const auto& movement = GetCharacterMovement();
		const bool onGround = movement.IsWalking();
		if (onGround)
		{
			myCoyoteTimeStamp = time;
			myJumpedSinceTouchedGround = false;
		}
		
		const auto wallrunState = Cast<UFPMovementStateWall>(GetState(UFPMovementStateWall::StaticClass()));
		const bool wallJump = wallrunState && wallrunState->GetCanWallJump(); 
	
		const bool onCoyoteGround = onGround ||
			((time - myCoyoteTimeStamp < myCoyoteTime) && !myJumpedSinceTouchedGround);

		if (onCoyoteGround || wallJump || HasAirJumps())
		{
			const FVector direction =
				wallrunState && wallJump ?
					wallrunState->GetWalljumpDirection() :
					FVector(0, 0, 1);
			Jump(direction);
			myJumpedSinceTouchedGround = true;
			if (wallJump && wallrunState)
				wallrunState->OnWallJump();
			if (!onCoyoteGround && !wallJump)
				myAirJumpCount++;
			return StaticClass();
		}
	}
	if (anAction == EFPMovementInputAction::JUMP_RELEASED)
		myJumpHeld = false;
		
	return nullptr;
}

UClass* UFPMovementStateInAir::OnLanded()
{
	ResetJumps();
	if (myJumpHeld)
	{
		// Bounce
		Jump();
		myJumpedSinceTouchedGround = true;
		return StaticClass();
	}
	return UFPMovementStateRun::StaticClass();
}

bool UFPMovementStateInAir::HasAirJumps() const
{
	return HasMagic() && (myAirJumpCount < myNumAirJumps);
}

void UFPMovementStateInAir::ResetJumps()
{
	myJumpedSinceTouchedGround = false;
	myCoyoteTimeStamp = GetCurrentTime();
	myAirJumpCount = 0;
}

TSubclassOf<UFPAnimationStateBase> UFPMovementStateInAir::GetAnimation() const
{
	return UFPAnimationStateInAir::StaticClass();
}
