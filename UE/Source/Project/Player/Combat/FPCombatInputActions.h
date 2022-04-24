#pragma once

#include "CoreMinimal.h"
#include "FPCombatInputActions.generated.h"

UENUM(BlueprintType)
enum class EFPCombatInput : uint8 {
	STRIKE,	
	BLOCK,
	INTERACT
};