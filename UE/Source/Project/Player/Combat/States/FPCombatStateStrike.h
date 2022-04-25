#pragma once

#include "CoreMinimal.h"
#include "FPCombatStateBase.h"
#include "FPCombatStateStrike.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPCombatStateStrike : public UFPCombatStateBase
{
	GENERATED_BODY()
	
public:
	
	virtual UClass* Update(float aDT) override;
	virtual UClass* Input(EFPCombatInput anAction) override;
	virtual void Enter() override;
	
	virtual int32 Priority() const override { return 2; }
	
	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual TSubclassOf<UFPAnimationStateBase> GetResetAnimation() const override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float myStrikeDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float myStrikeDamageDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly)
	float myStrikeCooldown = 0.5f;

private:

	bool myHasHit = false; 
	float myStrikeTimestamp = 0.0f; 
	
};
