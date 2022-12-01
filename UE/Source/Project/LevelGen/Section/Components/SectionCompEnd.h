#pragma once

#include "CoreMinimal.h"
#include "SectionCompBase.h"
#include "SectionCompEnd.generated.h"

class ASectionEnd;

UCLASS()
class PROJECT_API USectionCompEnd : public USectionCompBase
{
	GENERATED_BODY()

public:

	virtual TArray<int32> PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection) override;
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ASectionEnd>> myEnds;
	
};
