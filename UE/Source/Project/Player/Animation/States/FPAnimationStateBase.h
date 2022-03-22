#pragma once

#include "CoreMinimal.h"
#include "Project/Player/FPStateBase.h"
#include "FPAnimationStateBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPAnimationStateBase : public UFPStateBase
{
	GENERATED_BODY()
	
public:

	// Set / Get hand positions
	
	void SetTargetRight(const FTransform& aTransform);
	void SetRealRight(const FTransform& aTransform);
	FTransform GetTargetRight();
	FTransform GetRealRight();

	void SetTargetLeft(const FTransform& aTransform);
	void SetRealLeft(const FTransform& aTransform);
	FTransform GetTargetLeft();
	FTransform GetRealLeft();
	
};
