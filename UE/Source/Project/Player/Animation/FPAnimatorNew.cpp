#include "FPAnimatorNew.h"

#include "Project/Player/Movement/FPMovement.h"
#include "States/FPAnimationStateBase.h"
#include "States/FPAnimationStateIdle.h"

UClass* UFPAnimatorNew::GetDefaultStateType()
{
	return UFPAnimationStateIdle::StaticClass();
}

void UFPAnimatorNew::TryOverrideState(UClass* aStateType)
{
	CHECK_RETURN(!aStateType);
	const auto state = GetState(aStateType);
	CHECK_RETURN(!state);
	const auto currentState = GetCurrentState();
	// Will still set state if equal priority!
	CHECK_RETURN(currentState && currentState->Priority() > state->Priority()) 
	SetState(state);
}
