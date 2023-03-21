#include "FPMovementStateBoostCrystal.h"

#include "FPMovementStateInAir.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/States/FPAnimationStateInAir.h"

UClass* UFPMovementStateBoostCrystal::Update(const float aDT)
{
	const auto timer = GetCurrentTime() - myTimeStamp;
	auto& movement = GetCharacterMovement();
	
	if (GetIsInFreeze())
	{
		const float part = 1.0f - timer / myFreezeTime;
		movement.Velocity = myStartVelocity * part;
		return nullptr;
	}

	movement.Velocity = myDirection * mySpeed * FVector(
		mySpeedMulHorizontal,
		mySpeedMulHorizontal,
		mySpeedMulVertical);

	if (timer > myFreezeTime + myReleaseTime)
		return UFPMovementStateInAir::StaticClass();
	
	return nullptr;
}

void UFPMovementStateBoostCrystal::Enter()
{
	myTimeStamp = GetCurrentTime();
	
	const auto& movement = GetCharacterMovement();
	if (movement.IsWalking())
		Jump(FVector(0.0f, 0.0f, 1.0f));
	
	myStartVelocity = movement.GetLastUpdateVelocity();

	const auto diff = (GetActorTransform().GetLocation() - myTarget).GetSafeNormal();
	GetIsInFreeze() ? myDirection = (myDirection + diff).GetSafeNormal() : myDirection = diff; 
		
	Super::Enter();
}

void UFPMovementStateBoostCrystal::Exit()
{
	Super::Exit();
}

bool UFPMovementStateBoostCrystal::GetIsInFreeze() const
{
	const auto timer = GetCurrentTime() - myTimeStamp;
	return timer < myFreezeTime;
}

TSubclassOf<UFPAnimationStateBase> UFPMovementStateBoostCrystal::GetAnimation() const
{
	return UFPAnimationStateInAir::StaticClass();
}
