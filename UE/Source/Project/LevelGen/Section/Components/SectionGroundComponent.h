#pragma once

#include "SectionComponentBase.h"
#include "SectionGroundComponent.generated.h"

UCLASS()
class PROJECT_API USectionGroundComponent : public USectionComponentBase
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
