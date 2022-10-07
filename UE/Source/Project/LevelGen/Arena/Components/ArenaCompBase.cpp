#include "ArenaCompBase.h"

void UArenaCompBase::Populate(AArenaGenerator* aArenaGenerator, const FArenaLayer& aLayer, const FArenaSection& aSection, const FArenaSubdivision& aSubdivision)
{
	BPPopulate(aArenaGenerator, aLayer, aSection, aSubdivision);
}

FVector2D UArenaCompBase::GetSectionLocation(const FArenaLayer& aLayer, const FArenaSubdivision& aSubdivision, const float anAnglePart, const float aRadiusPart)
{
	const FVector2D startCoord = FVector2D(
		FMath::Cos(aSubdivision.startAngle),
		FMath::Sin(aSubdivision.startAngle));
	const FVector2D endCoord = FVector2D(
		FMath::Cos(aSubdivision.endAngle),
		FMath::Sin(aSubdivision.endAngle));

	const FVector2D direction = FMath::Lerp(startCoord, endCoord, anAnglePart).GetSafeNormal();
	const float radius = FMath::Lerp(aLayer.startRadius, aLayer.endRadius, aRadiusPart);
	return direction * radius;
}
