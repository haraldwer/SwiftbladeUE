#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "StateBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UStateBase : public USceneComponent
{
	GENERATED_BODY()
	
public:
	UStateBase();
	virtual ~UStateBase() override = default;

	void SetOwner(class UStateMachine* anOwner) { myOwner = anOwner; }
	
	virtual void Init() {}
	virtual UClass* Update(float aDT) { return nullptr; }
	virtual UClass* Check() { return nullptr; }
	
	virtual void Enter() {}
	virtual void Exit() {}
	
	virtual int32 Priority() const { return 0; }

protected:
	UStateBase* GetCurrentState() const;
	UStateBase* GetState(UClass* aType) const;
	bool IsCurrentState() const;

	template <class T>
	T* GetState() const
	{
		return Cast<T>(GetState(T::StaticClass()));
	}
	
private:
	UPROPERTY()
	class UStateMachine* myOwner;
};
