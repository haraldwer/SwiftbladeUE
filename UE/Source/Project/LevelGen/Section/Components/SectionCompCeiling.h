#pragma once

#include "SectionCompBase.h"
#include "SectionCompCeiling.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompCeiling : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:

	UPROPERTY(EditAnywhere, meta=(EditCondition=myHasCeil))
	float myCeilingThickness = 5000.0f;
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface* myCeilingMaterial;
	
};
