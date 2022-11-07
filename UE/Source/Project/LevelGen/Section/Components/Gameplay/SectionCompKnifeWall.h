#pragma once

#include "../SectionCompBase.h"
#include "SectionCompKnifeWall.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompKnifeWall : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> myKnifeClass;

	UPROPERTY(EditAnywhere)
	double myMinWallSize = 400.0f;
};
