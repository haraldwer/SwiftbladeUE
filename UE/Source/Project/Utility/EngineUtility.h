
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EngineUtility.generated.h"

UCLASS()
class PROJECT_API UEngineUtility : public UObject
{
	GENERATED_BODY()

public:

	virtual ~UEngineUtility() override = default;

	UFUNCTION(BlueprintCallable)
	static bool IsShipping();

	UFUNCTION(BlueprintCallable)
	static bool IsInBaseLevel();
};
