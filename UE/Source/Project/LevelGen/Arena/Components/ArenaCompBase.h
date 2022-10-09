#pragma once

#include "CoreMinimal.h"
#include "Project/LevelGen/GeneratorCompBase.h"
#include "Project/LevelGen/Arena/ArenaDataStructs.h"
#include "ArenaCompBase.generated.h"

class AArenaGenerator;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class PROJECT_API UArenaCompBase : public UGeneratorCompBase
{
	GENERATED_BODY()

public:
	
	virtual void Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision);

	UFUNCTION(BlueprintImplementableEvent)
	void BPPopulate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision);

protected:
	
	// Helpers

	static FVector2D GetSectionLocation(const FArenaLayer& aLayer, const FArenaSubdivision& aSubdivision, const float anAnglePart, const float aRadiusPart, bool aRoundRadius);
	
};
