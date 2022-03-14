#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateRun.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateRun : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	virtual UClass* Update(float aDT) override;
};
