#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateInAir.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateInAir : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	virtual UClass* Update(float aDT) override;
	virtual UClass* Check() override;
	virtual UClass* Input(EFPMovementInputAction anAction, float aValue) override;
	virtual UClass* OnLanded() override;
	
	bool HasAirJumps() const;
	void ResetJumps();

	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual int32 Priority() const override { return 5; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "InAir")
	float myCoyoteTime = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "InAir")
	int myNumAirJumps = 1;
	UPROPERTY(EditDefaultsOnly, Category = "InAir")
	float myHeavyLandingAnimThreshold = 1400.0f;  
	
private:
	bool myJumpedSinceTouchedGround = false;
	float myCoyoteTimeStamp = 0.2f;
	int myAirJumpCount = 1;
	bool myJumpHeld = false;
};
