#include "FPMovementStateMachine.h"

#include "States/FPMovementStateIdle.h"

UClass* UFPMovementStateMachine::GetDefaultStateType()
{
	return UFPMovementStateIdle::StaticClass();
}

void UFPMovementStateMachine::Input(const EFPMovementInputAction& anAction, const float aValue, const bool aAll)
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

void UFPMovementStateMachine::OnLanded()
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

void UFPMovementStateMachine::OnHit(const FHitResult& aHit)
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
