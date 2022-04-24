#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateSlide.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateSlide : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:

	virtual void Enter() override;
	virtual void Exit() override;
	virtual UClass* Update(float aDT) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float myHandWobbleStrength = 10.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myHandWobbleSpeed = 0.6f;
	
	UPROPERTY(EditDefaultsOnly)
	float myCameraBobStrength = 3.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myCameraBobSpeed = 4.0f;
};
