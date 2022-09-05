#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelEndLocation.generated.h"

UCLASS()
class PROJECT_API ALevelEndLocation : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelEndLocation();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEndLocationSet();
	
};
