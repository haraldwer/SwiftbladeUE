#pragma once

#include "SectionCompGround.h"
#include "SectionCompGroundSlice.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompGroundSlice : public USectionCompGround
{
	GENERATED_BODY()

public:

	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;
	
protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> mySliceClass;
	
	UPROPERTY(EditAnywhere)
	float mySliceTilt = 0.0f;

	UPROPERTY(EditAnywhere)
	bool mySliceHeight = false;
	
};
