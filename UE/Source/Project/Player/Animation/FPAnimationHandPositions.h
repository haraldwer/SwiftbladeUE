#pragma once

#include "CoreMinimal.h"
#include "Project/Player/Actors/HandStateEnum.h"
#include "Project/Utility/Math/LerpTransform.h"
#include "FPAnimationHandPositions.generated.h"

USTRUCT()
struct FFPAnimationHandPositions
{
	GENERATED_BODY()
	
	FTransform myRight;
	FTransform myLeft;

	EHandState myRightHandState = EHandState::OPEN;
	EHandState myLeftHandState = EHandState::OPEN;
	
	float myPosInterpSpd = 10.0f;
	float myRotInterpSpd = 10.0f;

	static FFPAnimationHandPositions Interp(const FFPAnimationHandPositions& aCurrent, const FFPAnimationHandPositions& aTarget, const float aDT)
	{
		FFPAnimationHandPositions result;
		result.myLeft = LerpTransDelta(aCurrent.myLeft, aTarget.myLeft, aDT, aCurrent.myRotInterpSpd, aCurrent.myPosInterpSpd);
		result.myRight = LerpTransDelta(aCurrent.myRight, aTarget.myRight, aDT, aCurrent.myRotInterpSpd, aCurrent.myPosInterpSpd);
		result.myLeftHandState = aCurrent.myLeftHandState;
		result.myRightHandState = aCurrent.myRightHandState;
		return result;
	}
};