#pragma once

#include "CoreMinimal.h"
#include "FPCombatStateBase.h"
#include "FPCombatStateNoSword.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPCombatStateNoSword : public UFPCombatStateBase
{
	GENERATED_BODY()
	
public:
	
	virtual UClass* Update(float aDT) override;
	virtual UClass* Input(EFPCombatInput anAction) override;
	virtual void Enter() override;

	virtual int32 Priority() const override { return 4; }

	virtual TSubclassOf<UFPAnimationStateBase> GetResetAnimation() const override;
	
protected:

	UPROPERTY(EditDefaultsOnly)
	float mySwordPickupTimer = 0.2f; 

private:
	
	float myTimer = 0.0f;
	
};
