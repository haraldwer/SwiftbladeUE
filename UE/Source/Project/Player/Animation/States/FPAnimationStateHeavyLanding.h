#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateHeavyLanding.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateHeavyLanding : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:

	virtual void Enter() override;
	virtual UClass* Update(float aDT) override;
	virtual int32 Priority() const override { return 1; }

protected:

	UPROPERTY(EditDefaultsOnly)
	float myDuration = 0.3f;

	UPROPERTY(EditDefaultsOnly)
	float myHeight = -50.0f;

	UPROPERTY(EditDefaultsOnly)
	float myTilt = 0.1f; 
	
};
