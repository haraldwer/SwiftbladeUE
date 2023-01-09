#pragma once

#include "CoreMinimal.h"
#include "SectionCompBase.h"
#include "SectionCompSoulPath.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompSoulPath : public USectionCompBase
{
	GENERATED_BODY()

public:

	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

	UPROPERTY(EditAnywhere)
	int32 myPathPrecision = 3;
	
	UPROPERTY(EditAnywhere)
	float myPathPadding = 0.1f;

	UPROPERTY(EditAnywhere)
	float myGroundOffset = 200.0f;
	
};
