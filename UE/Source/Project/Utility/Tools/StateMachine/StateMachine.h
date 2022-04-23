#pragma once

#include "CoreMinimal.h"
#include "StateBase.h"
#include "Components/SceneComponent.h"
#include "StateMachine.generated.h"

UCLASS(ClassGroup=(Custom))
class PROJECT_API UStateMachine : public USceneComponent
{
	GENERATED_BODY()

public:
	UStateMachine();
	virtual ~UStateMachine() override = default;
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Override with default state
	virtual UClass* GetDefaultStateType();

	virtual bool SetState(UStateBase* aState);
	virtual bool TryOverrideState(UStateBase* aState);
	UStateBase* GetCurrentState() const { return myState.Get(); };

	template <class T>
	T* GetState() const;
	UStateBase* GetState(UClass* aType);

protected:
	TWeakObjectPtr<UStateBase> myState;
	TArray<TWeakObjectPtr<UStateBase>> myStates;
};

template <class T>
T* UStateMachine::GetState() const
{
	for (auto& state : myStates)
		if (auto ptr = Cast<T>(state.Get()))
			return ptr;
	return nullptr;
}
