#pragma once

#include "../SectionCompBase.h"
#include "SectionCompGroundSpikes.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompGroundSpikes : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> mySpikeClass;

	UPROPERTY(EditAnywhere)
	float mySize = 175.0f;

	UPROPERTY(EditAnywhere)
	bool myUseAlternatingOffset = true;
	
};
