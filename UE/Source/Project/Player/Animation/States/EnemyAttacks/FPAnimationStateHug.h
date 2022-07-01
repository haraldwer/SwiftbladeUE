#pragma once

#include "CoreMinimal.h"
#include "../FPAnimationStateBase.h"
#include "FPAnimationStateHug.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateHug : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:

	virtual void Enter() override;
	virtual UClass* Update(float aDT) override;

	// When player tries to wiggle out of hug
	void Wiggle(float aScale);

protected:

	// The min / max offset. X is camera tilt. 
	UPROPERTY(EditDefaultsOnly)
	FVector myWiggleOffset;

private:

	FVector myOffset;
	
};
