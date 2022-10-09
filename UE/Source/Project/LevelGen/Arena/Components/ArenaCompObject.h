#pragma once

#include "CoreMinimal.h"
#include "ArenaCompBase.h"
#include "ArenaCompObject.generated.h"

class AEnemySpawner;

UCLASS()
class PROJECT_API UArenaCompObject : public UArenaCompBase
{
	GENERATED_BODY()

public:

	virtual void Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision) override;

protected:

	UPROPERTY(EditAnywhere)
	float myMinRadiusPart = 0.0f;
	UPROPERTY(EditAnywhere)
	float myMaxRadiusPart = 0.0f;

	UPROPERTY(EditAnywhere)
	float myMinAnglePart = 0.5f;
	UPROPERTY(EditAnywhere)
	float myMaxAnglePart = 0.5f;

	UPROPERTY(EditAnywhere)
	float myMinHeight = 200.0f;
	UPROPERTY(EditAnywhere)
	float myMaxHeight = 200.0f;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> myClass;

	UPROPERTY(EditAnywhere)
	FTransform myTransform;
	
};
