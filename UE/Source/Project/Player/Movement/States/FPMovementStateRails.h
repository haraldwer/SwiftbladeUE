#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateRails.generated.h"

class URailComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateRails : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	
	virtual UClass* Update(float aDT) override;
	virtual UClass* Check() override;
	virtual UClass* Input(EFPMovementInputAction anAction, float aValue) override;

	virtual void Enter() override;
	virtual void Exit() override;

	virtual bool RequiresMagic() const override { return true; }
	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual int32 Priority() const override { return 6; }

	FTransform GetSplineTransform() const;

protected:
	
	UPROPERTY(EditDefaultsOnly)
	float myCooldown = 0.7f;
	
	UPROPERTY(EditDefaultsOnly)
	float myVerticalOffset = -110;
	
	UPROPERTY(EditDefaultsOnly)
	float myRailVelocity = 700.0f;

	UPROPERTY(EditDefaultsOnly)
	float myInterpSpeed = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float myInputInterpSpeed = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	float myInitialVelOffsetMul = 0.5f;

private:
	
	TWeakObjectPtr<URailComponent> myRail;
	TWeakObjectPtr<URailComponent> myPrevRail;

	float myExitTimestamp = 0.0f;
	float mySplineDist = 0.0f;
	FVector myInterpInput = FVector::ZeroVector;
	
};
