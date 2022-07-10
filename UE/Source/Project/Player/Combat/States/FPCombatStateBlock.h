#pragma once

#include "CoreMinimal.h"
#include "FPCombatStateBase.h"
#include "FPCombatStateBlock.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPCombatStateBlock : public UFPCombatStateBase
{
	GENERATED_BODY()
	
public:

	virtual void Init() override;
	virtual UClass* Update(float aDT) override;
	virtual UClass* Input(EFPCombatInput anAction) override;
	virtual void Enter() override;

	virtual bool TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator, AActor* aDamageCauser) override;
	
	virtual int32 Priority() const override { return 1; }

	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual TSubclassOf<UFPAnimationStateBase> GetResetAnimation() const override;
	
protected:

	UPROPERTY(EditDefaultsOnly)
	float myBlockDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float myBlockCooldown = 0.2f;

private:
 
	float myBlockTimestamp = 0.0f; 
	
};
