#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateWallrun.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateWallrun : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:
	
	virtual UClass* Update(float aDT) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float myStepSize = 65.0f;

	UPROPERTY(EditDefaultsOnly)
	float mySwingAmount = 30.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float mySwingHandRotateAmount = 60.0f;

	UPROPERTY(EditDefaultsOnly)
	FVector mySwingVector = FVector(1.0f, 0.0f, 0.5f);

	UPROPERTY(EditDefaultsOnly)
	float myHeadBobStrength = 5.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myHeadTiltAmount = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	FTransform myExtendedHandTrans;

	UPROPERTY(EditDefaultsOnly)
	float myHeadWallTilt = 2.0f;

private:
	float myDist = 0.0f;
};
