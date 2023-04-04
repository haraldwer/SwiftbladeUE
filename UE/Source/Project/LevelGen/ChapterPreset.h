#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ChapterPreset.generated.h"

class USectionPreset; 

UCLASS(Blueprintable)
class UChapterPreset : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<USectionPreset>> mySections;

	UPROPERTY(EditDefaultsOnly)
	FString myStart = "SL_Start";

	UPROPERTY(EditDefaultsOnly)
	FString myEnd = "SL_End";
	
};


