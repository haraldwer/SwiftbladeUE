#pragma once

#include "../SectionCompBase.h"
#include "SectionCompSpikeLine.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompSpikeLine : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> mySpikeClass;
	
};
