#pragma once

#include "../SectionComponentBase.h"
#include "SectionSpikeLineComponent.generated.h"

UCLASS()
class PROJECT_API USectionSpikeLineComponent : public USectionComponentBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> mySpikeClass;
	
};
