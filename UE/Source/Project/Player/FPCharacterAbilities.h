#pragma once

#include "CoreMinimal.h"
#include "FPCharacterAbilities.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFPCharacterAbilities: uint8
{
	NONE	 = 0 UMETA(Hidden),
	WALLRUN	 = 1 << 0,
	DASH	 = 1 << 1,
	GRAPPLE	 = 1 << 2,
	RAILS	 = 1 << 3,
	BOOSTPAD = 1 << 4
};

ENUM_CLASS_FLAGS(EFPCharacterAbilities);