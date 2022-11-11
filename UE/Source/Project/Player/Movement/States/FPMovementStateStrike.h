#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateStrike.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateStrike : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	
	virtual UClass* Update(float aDT) override;
	virtual void Enter() override;
	virtual void Exit() override;

	virtual bool RequiresMagic() const override { return true; }
	virtual int32 Priority() const override { return 9; }

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Strike")
	TObjectPtr<class UCurveFloat> myVelocityCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Strike")
	float myDuration = 1.0f; 

private:

	FVector myOriginalVelocity = FVector::ZeroVector;
	float myEnterTimestamp = -1.0f;
	
};
