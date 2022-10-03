#pragma once

#include "SectionCompBase.h"
#include "SectionCompProp.generated.h"

USTRUCT()
struct FSectionProp
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> myClass;
	UPROPERTY(EditDefaultsOnly)
	float mySpawnWeight = 1.0f; // 1 == as common as any other prop
	UPROPERTY(EditDefaultsOnly)
	float myHeight = 0.0f;
	UPROPERTY(EditDefaultsOnly)
	float myPadding = 0.0f;
};

UCLASS()
class PROJECT_API USectionCompProp : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float myMinFillRate = 0.0f;
	UPROPERTY(EditDefaultsOnly)
	float myMaxFillRate = 10.0f;
	UPROPERTY(EditDefaultsOnly)
	TArray<FSectionProp> myProps;
	
};
