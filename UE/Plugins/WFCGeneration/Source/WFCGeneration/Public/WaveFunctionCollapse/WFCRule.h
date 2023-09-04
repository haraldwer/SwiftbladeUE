#pragma once

#include "CoreMinimal.h"
#include "WFCStructure.h"
#include "UObject/Object.h"
#include "WFCRule.generated.h"

UCLASS(Blueprintable)
class WFCGENERATION_API UWFCRule : public UObject
{
	GENERATED_BODY()

public:
	
	virtual bool Evaluate(const AWFCStructure* aStructure, const int32 anIndex)
	{
		return EvaluateBP(aStructure, anIndex);
	}

	UFUNCTION(BlueprintImplementableEvent)
	bool EvaluateBP(const AWFCStructure* aStructure, int32 anIndex); 
	
};
