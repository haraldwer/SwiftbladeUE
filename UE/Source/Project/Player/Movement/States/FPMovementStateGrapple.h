#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateGrapple.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateGrapple : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	virtual UClass* Update(float aDT) override;
	virtual UClass* Input(EFPMovementInputAction anAction, float aValue) override;

	virtual void Enter() override;
	virtual void Exit() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myDist = 2048.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myDot = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myCooldown = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myReleaseDist = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myReleaseTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myFreezeTime = 0.15f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float mySpeed = 1024.0f; 
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float mySpeedMulHorizontal = 0.9f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float mySpeedMulVertical = 1.7f;
	
private:
	FVector myTargetLocation;
	float myTimeStamp;
	FVector myStartVelocity;
	
};
