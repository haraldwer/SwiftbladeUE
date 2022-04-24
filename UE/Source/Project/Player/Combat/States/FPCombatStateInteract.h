#pragma once

#include "CoreMinimal.h"
#include "FPCombatStateBase.h"
#include "FPCombatStateInteract.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPCombatStateInteract : public UFPCombatStateBase
{
	GENERATED_BODY()
	
public:
	
	virtual UClass* Update(float aDT) override;
	virtual UClass* Input(EFPCombatInput anAction) override;
	
	virtual int32 Priority() const override { return 3; }
	
	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual TSubclassOf<UFPAnimationStateBase> GetResetAnimation() const override;
	
};
