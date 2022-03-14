#pragma once
#include "CoreMinimal.h"
#include "FPMovementInputAction.generated.h"

UENUM()
enum class EFPMovementInputAction : uint8
{
	// Only performed on current state
	MOVE_HORIZONTAL,
	MOVE_VERTICAL,
	LOOK_HORIZONTAL,
	LOOK_VERTICAL,

	// Performed on all states
	JUMP_PRESSED,
	JUMP_RELEASED,
	CROUCH_PRESSED,
	CROUCH_RELEASED,
	DASH,
	GRAPPLE
};
