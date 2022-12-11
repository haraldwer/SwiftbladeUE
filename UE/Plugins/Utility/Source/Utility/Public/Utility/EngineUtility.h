
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EngineUtility.generated.h"

UCLASS(ClassGroup=(Utility))
class UTILITY_API UEngineUtility : public UObject
{
	GENERATED_BODY()

public:

	virtual ~UEngineUtility() override = default;

	UFUNCTION(BlueprintCallable)
	static bool IsShipping();
	
};
