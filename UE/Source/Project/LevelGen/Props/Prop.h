#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prop.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UProp : public UActorComponent
{
	GENERATED_BODY()

public:
	UProp();
	
};
