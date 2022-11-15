#pragma once

#include "SectionCompBase.h"
#include "SectionCompProp.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompProp : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UPropConfig>> myConfigs;
	
};
