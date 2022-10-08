#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateGrapple.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateGrapple : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:
	virtual void Enter() override;
	virtual void Exit() override;
	virtual UClass* Update(float aDT) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float myHandWobbleStrength = 2.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myHandWobbleSpeed = 1.0f;
	
	UPROPERTY(EditDefaultsOnly)
	FQuat myExtendedRotation;

	UPROPERTY(EditDefaultsOnly)
	FVector myExtendCompensation = FVector(1.0f, 0.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly)
	FTransform myFinalTransform;

	UPROPERTY(EditDefaultsOnly)
	float myExtendedDist = 80.0f;
	
	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* myMagicEffectCurve;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEffect> myEffectType;

	UPROPERTY(EditDefaultsOnly)
	float myEffectDelay = 0.1f;

private:

	void UpdateMagicEffects(const FVector& aDirection);

	bool myHasCreatedEffect = false;

};
