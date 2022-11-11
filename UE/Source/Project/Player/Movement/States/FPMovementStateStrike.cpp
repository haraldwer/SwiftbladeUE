#include "FPMovementStateStrike.h"

#include "FPMovementStateInAir.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/States/FPAnimationStateDash.h"

UClass* UFPMovementStateStrike::Update(float aDT)
{
	const UCurveFloat* velCurve = myVelocityCurve.Get();
	CHECK_RETURN_LOG(!velCurve, "No gravity curve", UFPMovementStateInAir::StaticClass());
	
	const float diff = GetCurrentTime() - myEnterTimestamp;
	CHECK_RETURN(diff > myDuration, UFPMovementStateInAir::StaticClass());

	const float velMul = velCurve->GetFloatValue(diff / myDuration);
	GetCharacterMovement().Velocity = myOriginalVelocity * velMul; 
	
	return nullptr;
}

void UFPMovementStateStrike::Enter()
{
	myEnterTimestamp = GetCurrentTime();
	myOriginalVelocity = GetCharacterMovement().Velocity;

	ResetAbilities();
}

void UFPMovementStateStrike::Exit()
{
	GetCharacterMovement().Velocity = myOriginalVelocity; 
}
