#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateDash.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateDash : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:

	virtual void Enter() override;
	virtual UClass* Update(float aDT) override;
	
};
