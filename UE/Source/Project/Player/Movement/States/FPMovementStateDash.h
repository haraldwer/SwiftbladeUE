#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateDash.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateDash : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	virtual UClass* Update(float aDT) override;
	virtual UClass* Input(EFPMovementInputAction anAction, float aValue) override;
	
	virtual void Enter() override;
	virtual void Exit() override;

	virtual UClass* OnLanded() override;

	void Reset();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float myDashVelocity = 1500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float myDashVerticalMul = 0.25f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float myDashDuration = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float myDashFov = 5.0f;

private:
	FVector myDashDirection;
	bool myCanDash = true;
	float myDashTimestamp = 0.0f;
};
