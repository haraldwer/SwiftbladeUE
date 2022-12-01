#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelEnd.generated.h"

UCLASS()
class PROJECT_API ALevelEnd : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEndLocationSet(class ASectionGenerator* aGenerator);
	
};
