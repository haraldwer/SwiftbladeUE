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
	virtual void Enter() override;
	
	virtual int32 Priority() const override { return 3; }

protected:

	UPROPERTY(EditDefaultsOnly)
	float myInterpSpdIncrease = 200.0f;

	UPROPERTY(EditDefaultsOnly)
	float myMaxInterpSpd = 100.0f;
	
private:
	
	float myInterpSpd = 0.0f; 
};
