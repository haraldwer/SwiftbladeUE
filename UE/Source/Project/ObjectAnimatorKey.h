#pragma once

#include "CoreMinimal.h"
#include "ObjectAnimatorKey.generated.h"

USTRUCT()
struct FObjectAnimatorKey
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	int myObjectIndex = 0;
	UPROPERTY(EditDefaultsOnly)
	int myTransformIndex = -1;
	UPROPERTY(EditDefaultsOnly)
	float mySpeed = 10.0f; 
};