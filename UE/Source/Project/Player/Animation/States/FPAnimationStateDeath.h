﻿#pragma once

#include "CoreMinimal.h"
#include "FPAnimationStateBase.h"
#include "FPAnimationStateDeath.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateDeath : public UFPAnimationStateBase
{
	GENERATED_BODY()

public:

	virtual void Enter() override;
	virtual UClass* Update(float aDT) override;
	virtual int32 Priority() const override { return 1; }

	UPROPERTY(EditDefaultsOnly)
	float myDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	float myInitialLerpTime = 0.5f;
	
};
