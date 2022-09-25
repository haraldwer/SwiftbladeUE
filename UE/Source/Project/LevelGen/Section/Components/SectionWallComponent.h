#pragma once

#include "SectionComponentBase.h"
#include "SectionWallComponent.generated.h"

class USectionPropConfig;

UCLASS(Abstract)
class PROJECT_API USectionWallComponent : public USectionComponentBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:

	UPROPERTY(EditAnywhere)
	int32 myMinNumWalls = -1;
	
	UPROPERTY(EditAnywhere)
	int32 myMaxNumWalls = -1;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateWall(ASectionGenerator* aGenerator, const TArray<FVector2D>& aArray, int32 anIndex, float aGroundOffset, float aCeilHeight);
	
};
