#include "FPMovement.h"

#include "Project/Player/FPCharacter.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateBase.h"
#include "States/FPMovementStateIdle.h"

UClass* UFPMovement::GetDefaultStateType()
{
	return UFPMovementStateIdle::StaticClass();
}

bool UFPMovement::SetState(UStateBase* aState)
{
	// Call base function
	CHECK_RETURN(!Super::SetState(aState), false);

	// Update animator
	const auto character = Cast<AFPCharacter>(GetOwner());
	CHECK_RETURN_LOG(!character, "No character", true);
	UFPAnimatorNew* animator = character->GetAnimator();
	CHECK_RETURN_LOG(!animator, "No animator", true);
	const auto* state = Cast<UFPMovementStateBase>(aState);
	CHECK_RETURN(!state, true);
	const auto animation = state->GetAnimation();
	const auto animationClass = animation.Get();
	CHECK_RETURN_LOG(!animationClass, "State is missing animation class", true);
	animator->TryOverrideState(animator->GetState(animationClass));
	
	return true;
}

void UFPMovement::Input(const EFPMovementInputAction& anAction, const float aValue, const bool aAll)
{
	UStateBase* nextState = nullptr;
	if (aAll)
	{
		for (auto& it : myStates)
			if (const auto ptr = Cast<UFPMovementStateBase>(it.Get()))
				if (const auto newStateType = ptr->Input(anAction, aValue))
					if (const auto newState = GetState(newStateType))
						nextState = newState;
	}
	else
	{
		if (const auto currentState = Cast<UFPMovementStateBase>(GetCurrentState()))
			if (const auto newStateType = currentState->Input(anAction, aValue))
				if (const auto newState = GetState(newStateType))
					nextState = newState;
	}
	if (nextState)
		SetState(nextState);
}

void UFPMovement::OnLanded()
{
	UStateBase* nextState = nullptr;
	for (auto& it : myStates)
		if (const auto ptr = Cast<UFPMovementStateBase>(it.Get()))
			if (const auto newStateType = ptr->OnLanded())
				if (const auto newState = GetState(newStateType))
					nextState = newState;
	if (nextState)
		SetState(nextState);
}

void UFPMovement::OnHit(const FHitResult& aHit)
{
	UStateBase* nextState = nullptr;
	for (auto& it : myStates)
		if (const auto ptr = Cast<UFPMovementStateBase>(it.Get()))
			if (const auto newStateType = ptr->OnHit(aHit))
				if (const auto newState = GetState(newStateType))
					nextState = newState;
	if (nextState)
		SetState(nextState);
}
