#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SectionPreset.generated.h"

USTRUCT()
struct FSectionPresetRoom
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString myLevelName;

	// TODO: Weighted randomness
	UPROPERTY(EditAnywhere)
	float myWeight = 1.0f; 
};

UCLASS(Blueprintable)
class PROJECT_API USectionPreset : public UObject
{
	GENERATED_BODY()

public: 
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FSectionPresetRoom> myRooms;
 
	UPROPERTY(EditDefaultsOnly)
	int32 myNumRoomsToUse = -1;

	UPROPERTY(EditDefaultsOnly)
	FString myArena = "SL_Arena_00";
	
	UPROPERTY(EditDefaultsOnly)
	FString mySectionStart = "SL_Section_Start";

	UPROPERTY(EditDefaultsOnly)
	FString mySectionEnd = "SL_Section_End"; 
	
};
