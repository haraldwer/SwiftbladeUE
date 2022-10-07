#pragma once

#include "CoreMinimal.h"
#include "ArenaCompBase.h"
#include "ArenaCompWall.generated.h"

UCLASS()
class PROJECT_API UArenaCompWall : public UArenaCompBase
{
	GENERATED_BODY()

public:

	virtual void Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision) override;

protected:

	UPROPERTY(EditAnywhere)
	float myRadiusPart = 1.0f;

	UPROPERTY(EditAnywhere)
	float myAnglePadding = 0.0f;

	UPROPERTY(EditAnywhere)
	float myMinWallHeight = 500.0f;
	UPROPERTY(EditAnywhere)
	float myMaxWallHeight = 500.0f;

	UPROPERTY(EditAnywhere)
	float myMinWallGroundOffset = 0.0f;
	UPROPERTY(EditAnywhere)
	float myMaxWallGroundOffset = 0.0f;

	UPROPERTY(EditAnywhere)
	bool mySubdivideWall = false;
	
	UPROPERTY(EditAnywhere)
	int32 myWallSubdivisions = 16;
	
	UFUNCTION(BlueprintImplementableEvent)
	void CreateWall(AArenaGenerator* aGenerator, const TArray<FVector2D>& aArray, float aGroundOffset, float aWallHeight);

};
