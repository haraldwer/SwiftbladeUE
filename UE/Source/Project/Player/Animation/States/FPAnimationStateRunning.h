#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateRunning.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateRunning : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:
	
	virtual UClass* Update(float aDT) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float myHandWobbleStrength = 2.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myHandWobbleSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float myStepSize = 300.0f;

	UPROPERTY(EditDefaultsOnly)
	float mySwingAmount = 300.0f;

	UPROPERTY(EditDefaultsOnly)
	float myHeadBobStrength = 5.0f;
	UPROPERTY(EditDefaultsOnly)
	float myHeadTiltAmount = 5.0f;

private:
	float myDist = 0.0f;
};
