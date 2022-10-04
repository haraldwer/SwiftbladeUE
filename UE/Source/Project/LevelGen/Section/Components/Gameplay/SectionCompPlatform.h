#pragma once

#include "../SectionCompBase.h"
#include "SectionCompPlatform.generated.h"

class USectionPropConfig;

UCLASS(Abstract)
class PROJECT_API USectionCompPlatform : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> myPlatformClasses;

	UPROPERTY(EditAnywhere)
	float myMinHeight = -100.0f;

	UPROPERTY(EditAnywhere)
	float myMaxHeight = 100.0f;

	UPROPERTY(EditAnywhere)
	int32 myCount = 3;
	
};
