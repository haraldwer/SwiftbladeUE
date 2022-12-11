
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameUtility.generated.h"

UCLASS()
class PROJECT_API UGameUtility : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	static bool IsInBaseLevel();

	UFUNCTION(BlueprintPure)
	static bool IsAuthenticated();
	
};
