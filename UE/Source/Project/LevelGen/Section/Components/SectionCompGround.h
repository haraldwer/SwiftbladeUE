#pragma once

#include "SectionCompBase.h"
#include "SectionCompGround.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompGround : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:

	UPROPERTY(EditAnywhere, meta=(EditCondition=myHasGround))
	float myGroundThickness = 5000.0f;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition=myHasGround))
	UMaterialInterface* myGroundMaterial;
	
};
