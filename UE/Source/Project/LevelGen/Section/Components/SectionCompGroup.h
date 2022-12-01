#pragma once

#include "SectionCompBase.h"
#include "SectionCompGroup.generated.h"

UCLASS(Abstract)
class PROJECT_API USectionCompGroup : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	TArray<FGeneratorCompEntry<USectionCompBase>> GetSubComponents(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) const;
	
protected:

	UPROPERTY(EditDefaultsOnly)
	int32 myMinNumComps = 1;
	
	UPROPERTY(EditDefaultsOnly)
	int32 myMaxNumComps = 1;

	UPROPERTY(EditDefaultsOnly)
	bool myReuse = false;
	
	UPROPERTY(Instanced, EditDefaultsOnly)
	TArray<TObjectPtr<USectionCompBase>> myComponents;
	
};
