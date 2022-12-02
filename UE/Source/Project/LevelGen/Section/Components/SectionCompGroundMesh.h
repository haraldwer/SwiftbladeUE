#pragma once

#include "SectionCompGround.h"
#include "SectionCompGroundMesh.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompGroundMesh : public USectionCompGround
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:

	UPROPERTY(EditAnywhere)
	float myGroundThickness = 5000.0f;
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface* myGroundMaterial;
	
};
