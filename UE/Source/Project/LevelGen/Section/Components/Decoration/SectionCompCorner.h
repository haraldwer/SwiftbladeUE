#pragma once

#include "../SectionCompBase.h"
#include "SectionCompCorner.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompCorner : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;
	virtual void PlaceObject(ASectionGenerator* aGenerator, const FTransform& aTransform, const FProcRoom& aRoom);

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> myObjectType;

	UPROPERTY(EditAnywhere)
	float myCeilPart = 0.0f;

	UPROPERTY(EditAnywhere)
	bool myScaleHeight = false;

	UPROPERTY(EditAnywhere)
	bool myAvoidDoorways = false;

	UPROPERTY(EditAnywhere)
	float myOffset = 0.0f; 
};
