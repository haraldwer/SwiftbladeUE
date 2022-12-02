#include "Breakable.h"

UBreakable::UBreakable()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBreakable::Break(const FVector& aPosition, const FVector& aNormal)
{
	BreakMeshes(aPosition, aNormal);
	if (myDestroyOnBreak)
		if (const auto owner = GetOwner())
			owner->Destroy();
}