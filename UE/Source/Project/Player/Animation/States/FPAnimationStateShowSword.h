#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateShowSword.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateShowSword : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:

	virtual void Enter() override;
	virtual UClass* Update(float aDT) override;
	virtual int32 Priority() const override { return 1; }

	UPROPERTY(EditDefaultsOnly)
	float myDuration = 1.0f;
	
};
