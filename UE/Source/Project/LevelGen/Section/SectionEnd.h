#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SectionEnd.generated.h"

UCLASS()
class PROJECT_API ASectionEnd : public AActor
{
	GENERATED_BODY()

public:
	
	ASectionEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnLocationSet();
	
};
