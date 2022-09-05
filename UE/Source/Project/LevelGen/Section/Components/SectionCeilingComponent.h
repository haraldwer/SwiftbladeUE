#pragma once

#include "SectionComponentBase.h"
#include "SectionCeilingComponent.generated.h"

UCLASS()
class PROJECT_API USectionCeilingComponent : public USectionComponentBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	bool myHasCeil = true;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition=myHasCeil))
	float myCeilingThickness = 5000.0f;
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface* myCeilingMaterial;
	
	virtual void Populate(ASectionGenerator* aGenerator, const FProcSection& aSection) override;
	
};
