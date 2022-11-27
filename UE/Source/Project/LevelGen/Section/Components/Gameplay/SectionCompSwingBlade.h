#pragma once

#include "../SectionCompBase.h"
#include "SectionCompSwingBlade.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompSwingBlade : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> myBladeClass;

	UPROPERTY(EditAnywhere)
	float myWallDist = 200.0f;

	UPROPERTY(EditAnywhere)
	float myMinHeight = 200.0f; 
	
};
