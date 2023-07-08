#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateVault.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateVault : public UFPMovementStateBase
{
	GENERATED_BODY()

public:
	
	virtual UClass* Update(float aDT) override;
	virtual UClass* Check() override;
	virtual UClass* Input(EFPMovementInputAction anAction, float aValue) override;

	virtual void Enter() override;
	virtual void Exit() override;

	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual int32 Priority() const override { return 3; }

protected:

	UPROPERTY(EditDefaultsOnly)
	float myInputForwardMinDot = 0.3f;

	UPROPERTY(EditDefaultsOnly)
	float mySweepForwardOffset = 60.0f;

	UPROPERTY(EditDefaultsOnly)
	float mySweepStartZ = 60.0f;

	UPROPERTY(EditDefaultsOnly)
	float mySweepEndZ = -30.0f;

	UPROPERTY(EditDefaultsOnly)
	float mySweepRadius = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Crouch")
	float myCamHeightMul = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float myUpwardsVelocity = 1500.0f;

	UPROPERTY(EditDefaultsOnly)
	float myUpwardsSpeedup = 10.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myUpwardsSlowdown = 25.0f;

	UPROPERTY(EditDefaultsOnly)
	float myFlatVelRecorverySpeed = 25.0f;

	UPROPERTY(EditDefaultsOnly)
	float myExitDelay = 0.2f;
	
	UPROPERTY(EditDefaultsOnly)
	float myMaxExitVerticalSpeed = 200.0f; 

	UPROPERTY(EditDefaultsOnly)
	bool myCrouch = false; 

private:

	bool CheckLookConditions(); 
	UClass* CheckConditions();
	FHitResult Sweep() const;

	FHitResult myHit;
	FVector myInitialForward = FVector::ZeroVector;
	float myUpwardTimestamp = 0.0f; 

};
