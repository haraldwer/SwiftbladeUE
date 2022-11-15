#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameEnd.generated.h"

UCLASS(Blueprintable)
class PROJECT_API AGameEnd : public AActor
{
	GENERATED_BODY()

public:
	
	AGameEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void SetTime(float aScoreTime);
};
