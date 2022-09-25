#pragma once

#include "SectionComponentBase.h"
#include "SectionCeilingComponent.generated.h"

UCLASS()
class PROJECT_API USectionCeilingComponent : public USectionComponentBase
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
