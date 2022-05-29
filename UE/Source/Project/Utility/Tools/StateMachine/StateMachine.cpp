#include "StateMachine.h"
#include "StateBase.h"

UStateMachine::UStateMachine()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStateMachine::BeginPlay()
{
	Super::BeginPlay();

	const auto owner = GetOwner();
	CHECK_RETURN_LOG(!owner, "Owner nullptr");

	// Cache all states and set default state
	TArray<USceneComponent*> childComponents;
	GetChildrenComponents(false, childComponents);
	for (const auto& it : childComponents)
	{
		if (const auto state = Cast<UStateBase>(it))
		{
			myStates.Add(state);
			if (state->IsA(GetDefaultStateType()))
				SetState(state);
		}
	}

	// Sort states by priority
	myStates.Sort([](const TWeakObjectPtr<UStateBase>& aFirst, const TWeakObjectPtr<UStateBase>& aSecond) {
		return aFirst->Priority() < aSecond->Priority();
	});

	// Init
	for (auto& it : myStates)
	{
		if (const auto state = it.Get())
		{
			state->SetOwner(this); 
			state->Init();
		}
	}
}

void UStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Check conditions for every state
	// Prioritize later states
	UStateBase* nextState = nullptr;
	for (auto& state : myStates)
	{
		auto const ptr = state.Get();
		CHECK_CONTINUE_LOG(!ptr, "Invalid state ptr");
		if (const auto newStateType = ptr->Check())
			if (const auto newState = GetState(newStateType))
				nextState = newState;
	}

	if (nextState)
		SetState(nextState);

	// Update current state
	const auto statePtr = myState.Get();
	CHECK_RETURN_LOG(!statePtr, "No current state");
	if (const auto newStateType = statePtr->Update(DeltaTime))
		if (const auto newState = GetState(newStateType))
			SetState(newState);
}

UClass* UStateMachine::GetDefaultStateType()
{
	return UStateBase::StaticClass();
}

bool UStateMachine::SetState(UStateBase* aState)
{
	CHECK_RETURN(!aState, false);
	const auto currentState = myState.Get();
	CHECK_RETURN(aState == currentState, false);
	if (currentState)
		currentState->Exit();
	myState = aState;
	aState->Enter();
	return true;
}

bool UStateMachine::TryOverrideState(UStateBase* aState)
{
	CHECK_RETURN(!aState, false);
	const auto currentState = myState.Get();
	CHECK_RETURN(aState == currentState, false);
	// Will still set state if equal priority!
	CHECK_RETURN(currentState && currentState->Priority() > aState->Priority(), false); 
	return SetState(aState);
}

UStateBase* UStateMachine::GetState(UClass* aType)
{
	CHECK_RETURN(!aType, nullptr);
	for (auto& state : myStates)
		if (const auto ptr = state.Get())
			if (ptr->IsA(aType))
				return ptr;
	return nullptr;
}