#pragma once

#include "../Helpers/SectionCompPathSpline.h"
#include "SectionCompRail.generated.h"

class USectionPropConfig;

UCLASS(Abstract)
class PROJECT_API USectionCompRail : public USectionCompPathSpline
{
	GENERATED_BODY()

public:

	virtual FVector GetOffset() const override;

protected:

	UPROPERTY(EditAnywhere)
	float myRandHeight = 50.0f;
	
};
