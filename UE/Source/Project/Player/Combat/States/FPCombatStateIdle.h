#pragma once

#include "CoreMinimal.h"
#include "FPCombatStateBase.h"
#include "FPCombatStateIdle.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPCombatStateIdle : public UFPCombatStateBase
{
	GENERATED_BODY()
	
public:
	
	virtual UClass* Update(float aDT) override;
	
	virtual int32 Priority() const override { return 0; }
	
};
