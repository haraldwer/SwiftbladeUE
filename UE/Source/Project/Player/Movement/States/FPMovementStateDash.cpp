#include "FPMovementStateDash.h"

#include "FPMovementStateInAir.h"
#include "FPMovementStateRun.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/FPCamera.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Utility/Tools/CustomCamera.h"

UClass* UFPMovementStateDash::Update(float aDT)
{
	auto& movement = GetCharacterMovement();
	if (movement.IsWalking())
		myCanDash = true;

	const float timer = GetTime() - myDashTimestamp;  
	if (timer > myDashDuration)
		return movement.IsWalking() ?
			UFPMovementStateRun::StaticClass() :
			UFPMovementStateInAir::StaticClass();
	
	movement.Velocity = myDashDirection * myDashVelocity;
	
	// Dash camera effect
	const float dashIn = timer * 3.0f / myDashDuration;
	const float dashOut = FMath::Clamp((myDashDuration - timer) / myDashDuration, 0.0f, 1.0f);
	const float dashForwardMul = FMath::Clamp(FVector::DotProduct(movement.GetLastUpdateVelocity(),
		GetCharacter().GetActorForwardVector()), -1.0f, 1.0f);
	GetFPCamera().AddAdditiveFov(FMath::Min(dashIn, dashOut) * myDashFov * dashForwardMul);
	
	return nullptr;
}

UClass* UFPMovementStateDash::Input(EFPMovementInputAction anAction, float aValue)
{
	Super::Input(anAction, aValue);

	if (anAction == EFPMovementInputAction::DASH && myCanDash)
		return StaticClass();
	
	return nullptr;
}

void UFPMovementStateDash::Enter()
{
	auto& movement = GetCharacterMovement();
	auto vel = GetCamera().GetForwardVector();
	if (movement.IsWalking() && vel.Z < 0.1f)
		vel.Z = 0.1f;
	const auto input = GetCharacter().GetLastMovementInputVector();
	if(input.Size() > 0.3f)
		vel = FVector(input.X, input.Y, vel.Z);
	vel.Normalize();
	vel.Z += 0.1f * (1 - abs(vel.Z));
	vel.Z *= myDashVerticalMul;

	myDashDirection = vel;
	movement.Velocity = vel * myDashVelocity;
	myDashTimestamp = GetTime();
	myCanDash = false;
}

void UFPMovementStateDash::Exit()
{
	auto& movement = GetCharacterMovement();
	movement.Velocity = myDashDirection * movement.MaxWalkSpeed;
}

UClass* UFPMovementStateDash::OnLanded()
{
	myCanDash = true;
	return nullptr;
}

void UFPMovementStateDash::Reset()
{
	myCanDash = true; 
}
