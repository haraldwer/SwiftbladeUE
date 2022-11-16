#pragma once

#include "SectionCompBase.h"
#include "SectionCompCorner.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompCorner : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> myMesh;

	UPROPERTY(EditAnywhere)
	float myCeilPart = 0.0f;

	UPROPERTY(EditAnywhere)
	bool myScaleHeight = false; 
	
};
