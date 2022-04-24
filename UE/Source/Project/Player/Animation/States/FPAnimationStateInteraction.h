#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateInteraction.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateInteraction : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:
	
	virtual UClass* Update(float aDT) override;
	virtual int32 Priority() const override { return 3; }
};
