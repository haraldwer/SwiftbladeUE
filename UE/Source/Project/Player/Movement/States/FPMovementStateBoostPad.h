#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateBoostPad.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateBoostPad : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	
	virtual UClass* Update(float aDT) override;
	virtual UClass* Check() override;

	virtual void Enter() override;
	virtual void Exit() override;

	virtual bool RequiresMagic() const override { return true; }
	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual int32 Priority() const override { return 6; }

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Boost")
	float myBoostVelocity = 1500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Boost")
	float myBoostDuration = 0.4f;

private:
	
	FVector myBoostDirection;
	FVector myBoostLocation;
	float myBoostTimestamp = 0.0f;
};
