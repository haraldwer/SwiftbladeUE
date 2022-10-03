#pragma once

#include "CoreMinimal.h"
#include "SectionCompBase.h"
#include "SectionCompEnd.generated.h"

UCLASS()
class PROJECT_API USectionCompEnd : public USectionCompBase
{
	GENERATED_BODY()

public:

	virtual TArray<int32> PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection) override;
	
};
