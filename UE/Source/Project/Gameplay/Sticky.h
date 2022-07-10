#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sticky.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API USticky : public UActorComponent
{
	GENERATED_BODY()

public:
	
	USticky();
	
};
