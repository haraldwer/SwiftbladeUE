#include "FPMovement.h"

#include "GameFramework/PawnMovementComponent.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateBase.h"
#include "States/FPMovementStateIdle.h"

void UFPMovement::BeginPlay()
{
	Super::BeginPlay();
	
	const auto character = Cast<AFPCharacter>(GetOwner());
	CHECK_RETURN_LOG(!character, "Missing character");
	const auto moveComp = character->GetMovementComponent();
	CHECK_RETURN_LOG(!moveComp, "Missing move comp");
	moveComp->Velocity = FVector::ZeroVector;
}

void UFPMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	TryOverrideAnimationState();
}

UClass* UFPMovement::GetDefaultStateType()
{
	return UFPMovementStateIdle::StaticClass();
}

bool UFPMovement::SetState(UStateBase* aState)
{
	if (const auto moveState = Cast<UFPMovementStateBase>(aState))
		if (moveState->RequiresMagic())
			if (const auto character = Cast<AFPCharacter>(GetOwner()))
				if (!character->HasMagic())
					return false;
	
	// Call base function
	CHECK_RETURN(!Super::SetState(aState), false);
	TryOverrideAnimationState();
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

void UFPMovement::TryOverrideAnimationState() const
{
	const auto character = Cast<AFPCharacter>(GetOwner());
	CHECK_RETURN_LOG(!character, "No character");
	UFPAnimatorNew* animator = character->GetAnimator();
	CHECK_RETURN_LOG(!animator, "No animator");
	const auto* state = Cast<UFPMovementStateBase>(GetCurrentState());
	CHECK_RETURN(!state);
	const auto animation = state->GetAnimation();
	const auto animationClass = animation.Get();
	CHECK_RETURN(!animationClass);
	animator->TryOverrideState(animator->GetState(animationClass));
}
