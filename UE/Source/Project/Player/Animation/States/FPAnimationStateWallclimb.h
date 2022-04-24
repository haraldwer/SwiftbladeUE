#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateWallclimb.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateWallclimb : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:

	virtual UClass* Update(float aDT) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float mySwingAmount = 30.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float mySwingHandRotateAmount = 60.0f;

	UPROPERTY(EditDefaultsOnly)
	FVector mySwingVector = FVector(0.1f, 0.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly)
	float myHeadTiltAmount = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	float mySpeed = 10.0f;
};
