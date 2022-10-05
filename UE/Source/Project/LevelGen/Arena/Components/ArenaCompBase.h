#pragma once

#include "CoreMinimal.h"
#include "Project/LevelGen/Arena/ArenaDataStructs.h"
#include "ArenaCompBase.generated.h"

class AArenaGenerator;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class PROJECT_API UArenaCompBase : public UObject
{
	GENERATED_BODY()

public:
	
	virtual void Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision);

	UFUNCTION(BlueprintImplementableEvent)
	void BPPopulate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision);
	
};
