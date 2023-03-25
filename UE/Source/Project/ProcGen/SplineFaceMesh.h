#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineFaceMesh.generated.h"

class UProceduralMeshComponent;
class USplineComponent;

UCLASS()
class PROJECT_API ASplineFaceMesh : public AActor
{
	GENERATED_BODY()

public:

	ASplineFaceMesh();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(CallInEditor)
	void Generate() const;

protected:
	
	UPROPERTY(EditAnywhere)
	float myPrecision = 10.0f;

	UPROPERTY(EditAnywhere)
	float myHeight = 100.0f; 

	UPROPERTY(EditAnywhere)
	UMaterialInterface* myMaterial;

private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<USplineComponent> mySpline;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UProceduralMeshComponent> myMesh;
	
};
