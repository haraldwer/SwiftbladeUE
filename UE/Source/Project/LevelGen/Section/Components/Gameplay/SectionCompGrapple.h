#pragma once

#include "../SectionCompBase.h"
#include "SectionCompGrapple.generated.h"

class USectionPropConfig;

UCLASS(Abstract)
class PROJECT_API USectionCompGrapple : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGrapplePoint> myGrappleClass;

	UPROPERTY(EditAnywhere)
	float myHeight = 200.0f; 
	
};
