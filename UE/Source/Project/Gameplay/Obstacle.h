#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Obstacle.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UObstacle : public UActorComponent
{
	GENERATED_BODY()

public:
	UObstacle();
	
};
