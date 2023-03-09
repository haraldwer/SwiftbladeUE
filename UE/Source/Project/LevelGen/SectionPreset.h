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
};

UCLASS(Blueprintable)
class PROJECT_API USectionPreset : public UObject
{
	GENERATED_BODY()

public: 
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FSectionPresetRoom> myRooms;

	// TODO: 
	//UPROPERTY(EditDefaultsOnly)
	//int32 numRoomsToUse = -1;

	// TODO: 
	//UPROPERTY(EditDefaultsOnly)
	//int32 myMinDifficulty = -1;

	// TODO: 
	//UPROPERTY(EditDefaultsOnly)
	//int32 myMaxDifficulty = -1;

	// TODO: 
	//UPROPERTY(EditDefaultsOnly)
	//int32 myRequiredAbilities = -1;
	
};
