#pragma once

#include "../SectionComponentBase.h"
#include "SectionKnifeWallComponent.generated.h"

UCLASS()
class PROJECT_API USectionKnifeWallComponent : public USectionComponentBase
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
