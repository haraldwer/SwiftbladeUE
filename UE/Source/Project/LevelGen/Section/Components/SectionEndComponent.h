#pragma once

#include "CoreMinimal.h"
#include "SectionComponentBase.h"
#include "SectionEndComponent.generated.h"

UCLASS()
class PROJECT_API USectionEndComponent : public USectionComponentBase
{
	GENERATED_BODY()

public:

	virtual void Populate(ASectionGenerator* aGenerator, const FProcSection& aSection) override;
	
};
