#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateBoostCrystal.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateBoostCrystal : public UFPMovementStateBase
{
	GENERATED_BODY()

public:
	
	virtual UClass* Update(float aDT) override;

	virtual void Enter() override;
	virtual void Exit() override;

	void SetTargetLocation(const FVector& InTarget) { myTarget = InTarget; };
	bool GetIsInFreeze() const;

	virtual bool RequiresMagic() const override { return true; }
	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual int32 Priority() const override { return 7; }

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myFreezeTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myReleaseTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float mySpeed = 1024.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float mySpeedMulHorizontal = 0.9f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float mySpeedMulVertical = 1.3f;

private:
	
	FVector myTarget = FVector::ZeroVector;
	float myTimeStamp = 0.0f;
	FVector myStartVelocity = FVector::ZeroVector;
	FVector myDirection = FVector::ZeroVector;
};
