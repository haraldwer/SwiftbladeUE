#pragma once

#include "../SectionComponentBase.h"
#include "SectionGrappleComponent.generated.h"

class USectionPropConfig;

UCLASS(Abstract)
class PROJECT_API USectionGrappleComponent : public USectionComponentBase
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
