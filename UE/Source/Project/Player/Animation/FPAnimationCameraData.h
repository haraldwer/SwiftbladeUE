#pragma once

#include "CoreMinimal.h"
#include "FPAnimationCameraData.generated.h"

USTRUCT()
struct FFPAnimationCameraData
{
	GENERATED_BODY()

	float myHeight = 0.0f;
	float myTilt = 0.0f;
	float myFOV = 0.0f;

	float myHeightInterpSpd = 10.0f;
	float myTiltInterpSpd = 10.0f;
	float myFovInterpSpd = 10.0f; 

	static FFPAnimationCameraData Interp(const FFPAnimationCameraData& aCurrent, const FFPAnimationCameraData& aTarget, const float aDT)
	{
		FFPAnimationCameraData result;
		result.myHeight = FMath::FInterpTo(aCurrent.myHeight, aTarget.myHeight, aDT, aCurrent.myHeightInterpSpd);
		result.myTilt = FMath::FInterpTo(aCurrent.myTilt, aTarget.myTilt, aDT, aCurrent.myTiltInterpSpd);
		result.myFOV = FMath::FInterpTo(aCurrent.myFOV, aTarget.myFOV, aDT, aCurrent.myFovInterpSpd);
		return result; 
	}
};
