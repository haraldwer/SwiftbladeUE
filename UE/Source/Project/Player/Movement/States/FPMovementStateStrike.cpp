#include "FPMovementStateStrike.h"

#include "FPMovementStateInAir.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Animation/States/FPAnimationStateDash.h"

UClass* UFPMovementStateStrike::Update(float aDT)
{
	const UCurveFloat* gravCurve = myGravityCurve.Get();
	CHECK_RETURN_LOG(!gravCurve, "No gravity curve", UFPMovementStateInAir::StaticClass());
	
	const float diff = GetCurrentTime() - myEnterTimestamp;
	CHECK_RETURN(diff > myDuration, UFPMovementStateInAir::StaticClass());

	const float gravMul = gravCurve->GetFloatValue(diff / myDuration);
	GetCharacterMovement().GravityScale = myOriginalGravity * gravMul; 
	
	return nullptr;
}

void UFPMovementStateStrike::Enter()
{
	myEnterTimestamp = GetCurrentTime();
	myOriginalGravity = GetCharacterMovement().GravityScale;

	ResetAbilities();
}

void UFPMovementStateStrike::Exit()
{
	GetCharacterMovement().GravityScale = myOriginalGravity; 
}
