#include "ArenaCompBase.h"

void UArenaCompBase::Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision)
{
	BPPopulate(aArenaGenerator, aLayer, aSection, aSubdivision);
}
