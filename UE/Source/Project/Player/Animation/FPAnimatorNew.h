#pragma once

#include "CoreMinimal.h"
#include "Project/Utility/Tools/StateMachine/StateMachine.h"
#include "FPAnimatorNew.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimatorNew : public UStateMachine
{
	GENERATED_BODY()

public:
	virtual UClass* GetDefaultStateType() override;
	void TryOverrideState(UClass* aStateType);

};
