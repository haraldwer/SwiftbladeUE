#pragma once

#include "SectionComponentBase.h"
#include "SectionPropComponent.generated.h"

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

UCLASS(Blueprintable)
class USectionPropConfig : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly)
	float myMinFillRate = 0.0f;
	UPROPERTY(EditDefaultsOnly)
	float myMaxFillRate = 10.0f;
	UPROPERTY(EditDefaultsOnly)
	TArray<FSectionProp> myProps;
	
};

UCLASS()
class PROJECT_API USectionPropComponent : public USectionComponentBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<USectionPropConfig> myPropConfig;

	virtual void Populate(ASectionGenerator* aGenerator, const FProcSection& aSection) override;
	
};
