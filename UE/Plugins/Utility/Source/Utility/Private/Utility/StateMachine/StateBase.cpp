#include "Utility/StateMachine/StateBase.h"
#include "Utility/StateMachine/StateMachine.h"

UStateBase::UStateBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UStateBase* UStateBase::GetCurrentState() const
{
	CHECK_RETURN_LOG(!myOwner, "No owner", nullptr);
	return myOwner->GetCurrentState();
}

UStateBase* UStateBase::GetState(UClass* aType) const
{
	CHECK_RETURN_LOG(!myOwner, "No owner", nullptr);
	return myOwner->GetState(aType);
}

bool UStateBase::IsCurrentState() const
{
	return GetCurrentState() == this;
}

