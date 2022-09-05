#pragma once

#include "CoreMinimal.h"
#include "Project/LevelGen/Section/SectionDataStructs.h"
#include "SectionComponentBase.generated.h"

class ASectionGenerator;

UCLASS(Blueprintable)
class PROJECT_API USectionComponentBase : public UObject
{
	GENERATED_BODY()

public:
	
	virtual void Populate(ASectionGenerator* aGenerator, const FProcSection& aSection)
	{
		BPPopulate(aGenerator, aSection);
	}

	UFUNCTION(BlueprintImplementableEvent)
	void BPPopulate(ASectionGenerator* aGenerator, const FProcSection& aSection);
	
};
