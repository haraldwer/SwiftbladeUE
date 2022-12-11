#pragma once

#include "CoreMinimal.h"
#include "AuthenticationData.generated.h"

UENUM(BlueprintType)
enum class EAuthMethod : uint8
{
	DEVICE,
	// EMAIL,
	// STEAM,
	// GOOGLE,
};

USTRUCT(BlueprintType)
struct FAuthData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EAuthMethod myMethod = EAuthMethod::DEVICE;

	UPROPERTY(BlueprintReadWrite)
	FString myUsername;
	
	UPROPERTY(BlueprintReadWrite)
	FString myToken;
	
};
