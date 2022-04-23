#pragma once

#include "CoreMinimal.h"
#include "HandStateEnum.generated.h"

UENUM()
enum class EHandState : uint8
{
	OPEN,
	CLOSED
};