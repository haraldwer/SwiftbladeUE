#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PropConfig.generated.h"

USTRUCT()
struct FPropEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APropGroup> myGroup;

	// 1 == as common as any other group
	UPROPERTY(EditDefaultsOnly)
	float mySpawnWeight = 1.0f; 
};

UCLASS(Blueprintable)
class PROJECT_API UPropConfig : public UObject
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FPropEntry> myEntries;

	UPROPERTY(EditDefaultsOnly)
	float myFillRate = 1.0f;
	
};
