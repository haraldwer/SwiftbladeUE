#include "PropGroup.h"

APropGroup::APropGroup()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APropGroup::Generate()
{
	for (auto prop : myPropsToPlace)
	{
		
	}
	
	// Generate child prop groups
	TArray<AActor*> children;
	GetAllChildActors(children, false);
	for (const auto& child : children)
		if (const auto propGroup = Cast<APropGroup>(child))
			propGroup->Generate();

	
}
