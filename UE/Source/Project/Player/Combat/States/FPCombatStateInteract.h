#pragma once

#include "CoreMinimal.h"
#include "FPCombatStateBase.h"
#include "FPCombatStateInteract.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPCombatStateInteract : public UFPCombatStateBase
{
	GENERATED_BODY()
	
public:

	virtual void Init() override;
	virtual UClass* Update(float aDT) override;
	virtual UClass* Check() override;
	virtual UClass* Input(EFPCombatInput anAction) override;
	
	virtual int32 Priority() const override { return 3; }
	
	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual TSubclassOf<UFPAnimationStateBase> GetResetAnimation() const override;

	class UInteraction* GetInteraction() const { return myInteraction.Get(); }

protected:

	UPROPERTY(EditDefaultsOnly)
	float myRadius = 200.0f;

	UPROPERTY(EditDefaultsOnly)
	float myAcceptableDot = 0.3f;
	
private:

	UClass* CheckConditions();
	
	TArray<class UInteraction*> FindPossibleInteractions() const;
	bool IsCloseToSpecificInteraction(const class UInteraction* anInteraction) const;
	class UInteraction* FindClosestInteraction() const;
	
	TWeakObjectPtr<class UInteraction> myInteraction; 
	
};
