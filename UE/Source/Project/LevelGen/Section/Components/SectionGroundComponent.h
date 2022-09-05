#pragma once

#include "SectionComponentBase.h"
#include "SectionGroundComponent.generated.h"

UCLASS(BlueprintType)
class PROJECT_API USectionGroundComponent : public USectionComponentBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	bool myHasGround = true;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition=myHasGround))
	float myGroundThickness = 5000.0f;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition=myHasGround))
	UMaterialInterface* myGroundMaterial;
	
	virtual void Populate(ASectionGenerator* aGenerator, const FProcSection& aSection) override;
	
};
