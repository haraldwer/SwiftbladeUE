#pragma once

#include "SectionCompBase.h"
#include "SectionCompPathSpline.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompPathSpline : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

	virtual FVector GetOffset() const { return myOffset; }
	
protected:

	UPROPERTY(EditAnywhere)
	FVector myOffset = FVector(0.0f, 0.0f, 0.0f);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> myClass;
	
	UPROPERTY(EditAnywhere)
	int myPathPrecision = 4;
	
	UPROPERTY(EditAnywhere)
	float myPathPadding = 0.1f;
};
