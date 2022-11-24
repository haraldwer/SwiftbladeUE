#pragma once

#include "CoreMinimal.h"
#include "SectionCompPathSpline.h"
#include "UObject/Object.h"
#include "SectionCompCave.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompCave : public USectionCompBase
{
	GENERATED_BODY()

public:

	virtual TArray<int32> PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection) override;

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> myClass;
	
};
