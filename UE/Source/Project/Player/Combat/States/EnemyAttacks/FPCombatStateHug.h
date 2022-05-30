#pragma once

#include "CoreMinimal.h"
#include "../FPCombatStateBase.h"
#include "FPCombatStateHug.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPCombatStateHug : public UFPCombatStateBase
{
	GENERATED_BODY()
	
public:

	virtual void Enter() override;
	virtual void Exit() override;
	virtual UClass* Update(float aDT) override;
	virtual UClass* Input(EFPCombatInput anAction) override;

	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual TSubclassOf<UFPAnimationStateBase> GetResetAnimation() const override;
	
	virtual int32 Priority() const override { return 10; }
	
	bool HasReachedTarget() const { return myValue > myTargetValue; }
	
protected:

	UPROPERTY(EditDefaultsOnly)
	float myFriction = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float myAcceleration = 0.16f;

	UPROPERTY(EditDefaultsOnly)
	float myTargetValue = 100.0f;

private:

	float myVelocity = 0.0f;;
	float myValue = 0.0f;
};
