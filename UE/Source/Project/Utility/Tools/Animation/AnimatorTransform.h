﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimatorTransform.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UAnimatorTransform : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAnimatorTransform();

};
