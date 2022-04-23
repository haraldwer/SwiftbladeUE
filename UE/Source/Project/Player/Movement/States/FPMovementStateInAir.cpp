#include "FPMovementStateInAir.h"

#include "FPMovementStateRun.h"
#include "FPMovementStateWallrun.h"
#include "GameFramework/CharacterMovementComponent.h"

UClass* UFPMovementStateInAir::Update(float aDT)
{
	const auto& movement = GetCharacterMovement();
	if (movement.IsWalking())
		return UFPMovementStateRun::StaticClass();
	
	return nullptr;
}

UClass* UFPMovementStateInAir::Input(EFPMovementInputAction anAction, float aValue)
{
	Super::Input(anAction, aValue);

	if (anAction == EFPMovementInputAction::JUMP_PRESSED)
	{
		myJumpHeld = true;
		const float time = GetTime();
		const auto& movement = GetCharacterMovement();
		const bool onGround = movement.IsWalking();
		if (onGround)
			myCoyoteTimeStamp = time;

		const auto wallrunState = Cast<UFPMovementStateWallrun>(GetState(UFPMovementStateWallrun::StaticClass()));
		const bool wallJump = wallrunState && wallrunState->GetCanWallJump(); 
	
		const bool onCoyoteGround =
			onGround || (time - myCoyoteTimeStamp < myCoyoteTime);

		if (onCoyoteGround || wallJump || HasAirJumps())
		{
			const FVector direction =
			wallrunState && wallJump ?
				wallrunState->GetWalljumpDirection() :
				FVector(0, 0, 1);
			if (wallJump)
				LOG("Walljump");
			Jump(direction);
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
		return StaticClass();
	}
	return UFPMovementStateRun::StaticClass();
}

bool UFPMovementStateInAir::HasAirJumps() const
{
	return (myAirJumpCount < myNumAirJumps);
}

void UFPMovementStateInAir::ResetJumps()
{
	myCoyoteTimeStamp = GetTime();
	myAirJumpCount = 0;
}
