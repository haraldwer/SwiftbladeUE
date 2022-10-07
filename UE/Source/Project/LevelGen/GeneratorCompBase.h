#pragma once

#include "CoreMinimal.h"
#include "GeneratorCompBase.generated.h"

UCLASS(Abstract)
class PROJECT_API UGeneratorCompBase : public UObject
{
	GENERATED_BODY()

public:

	const TArray<TSubclassOf<UGeneratorCompBase>>& GetReqiredComps() const { return myRequiredComponents; }
	const TArray<TSubclassOf<UGeneratorCompBase>>& GetBlockingComps() const { return myBlockingComponents; }

	float GetChance() const { return myComponentChance; } 
	
protected:
	
	UPROPERTY(EditAnywhere)
	float myComponentChance = 100.0f;

	// Requirements
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGeneratorCompBase>> myRequiredComponents;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGeneratorCompBase>> myBlockingComponents;

	// Helper
	
	static FVector2D GetBlendVert(const TArray<FVector2D>& someVerts, float anIndex);
	static bool FitIntoMesh(FVector2D& aPosition, float aRadius, const TArray<FVector2D> aMesh, const FVector2D aMeshCenter, UWorld* aWorld);
	
};
