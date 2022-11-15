#pragma once

#include "SectionCompBase.h"
#include "SectionCompProp.generated.h"

class APropGroup;
class UPropConfig;
struct FPropEntry;

UCLASS(Abstract)
class PROJECT_API USectionCompProp : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

protected:
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UPropConfig>> myConfigs;
	
private:
	
	void CreateGroup(ASectionGenerator& aGenerator, const FProcRoom& aRoom, UPropConfig& aConfig) const;
	
	// Helpers
	static FVector2D GetPlacement(const FProcRoom& aRoom, const APropGroup& aGroupObject);
	static void AdjustPlacement(const FProcRoom& aRoom, const APropGroup& aGroupObject, FVector2D& aPlacement, FVector2D& aNormal);
	bool FindOverlaps(const ASectionGenerator& aGenerator, const APropGroup& aGroupObject, const FTransform& aTrans) const;
	static FPropEntry* SelectPropGroup(UPropConfig& aConfig);
	
};
