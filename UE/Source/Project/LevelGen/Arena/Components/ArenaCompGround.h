#pragma once

#include "CoreMinimal.h"
#include "ArenaCompBase.h"
#include "ArenaCompGround.generated.h"

UCLASS()
class PROJECT_API UArenaCompGround : public UArenaCompBase
{
	GENERATED_BODY()

public:

	virtual void Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision) override;

protected:
	
	UPROPERTY(EditAnywhere)
	float myGroundThickness = 5000.0f;
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface* myGroundMaterial;

};
