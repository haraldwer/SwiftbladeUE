#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateIdle.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateIdle : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:
	virtual UClass* Update(float aDT) override;
	
};
