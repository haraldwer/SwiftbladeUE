#pragma once

#include "CoreMinimal.h"
#include "HandStateEnum.generated.h"

UENUM(BlueprintType)
enum class EHandState : uint8
{
	OPEN,
	CLOSED
};