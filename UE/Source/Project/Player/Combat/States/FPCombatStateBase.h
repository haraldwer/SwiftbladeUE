#pragma once

#include "CoreMinimal.h"
#include "Project/Player/FPStateBase.h"
#include "Project/Player/Combat/FPCombatInputActions.h"
#include "FPCombatStateBase.generated.h"

class UFPAnimationStateBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPCombatStateBase : public UFPStateBase
{
	GENERATED_BODY()

public:
	
	virtual ~UFPCombatStateBase() override = default;
	virtual void Exit() override;
	
	virtual UClass* Input(EFPCombatInput anAction) { return nullptr; }
	
	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const { return {}; }
	virtual TSubclassOf<UFPAnimationStateBase> GetResetAnimation() const { return {}; }

protected:

	// Helpers
	class ASword* GetSword() const;
	
};
