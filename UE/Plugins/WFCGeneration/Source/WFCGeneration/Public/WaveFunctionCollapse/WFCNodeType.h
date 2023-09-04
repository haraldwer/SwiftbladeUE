#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCNodeType.generated.h"

class UWFCRule;

UCLASS(Blueprintable)
class WFCGENERATION_API AWFCNodeType : public AActor
{
	GENERATED_BODY()

	friend class AWFCStructure; 
	
protected:
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UWFCRule>> myRules;

};
