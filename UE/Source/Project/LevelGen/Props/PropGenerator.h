#pragma once

#include "CoreMinimal.h"
#include "Project/LevelGen/GeneratorBase.h"
#include "PropGenerator.generated.h"

class UArrowComponent;
class USplineComponent;
class UPropConfig;
class APropGroup;
struct FPropEntry; 

UCLASS()
class PROJECT_API APropGenerator : public AGeneratorBase
{
	GENERATED_BODY()

public:
	
	APropGenerator();

	virtual void BeginPlay() override;
	virtual void Generate(ALevelManager* aLevelManager) override;

protected:
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UPropConfig>> myConfigs;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USplineComponent> mySpline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UArrowComponent> myTop;

	UPROPERTY(EditAnywhere)
	int32 mySubdivisions = 32;
	
	UPROPERTY(EditAnywhere)
	int mySplinePlacementPrecision = 2;

private:
	
	static FPropEntry* SelectPropGroup(UPropConfig& aConfig);
	void CreateGroup(UPropConfig& aConfig);
	FVector2D GetPlacement(const APropGroup& aGroupObject) const;
	void AdjustPlacement(const APropGroup& aGroupObject, FVector2D& aPlacement, FVector2D& aNormal);
	bool FindOverlaps(const APropGroup& aGroupObject, const FTransform& aTrans) const;
	
	FVector2D GetCenter() const; 
	
	void CalculateArea();
	struct FPropArea
	{
		TArray<FVector2D> myVerts;
		TArray<FVector2D> myCorners;
		float myRadius = 0.0f;
	} myArea; 
};
