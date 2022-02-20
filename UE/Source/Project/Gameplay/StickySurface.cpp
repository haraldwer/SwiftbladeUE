#include "StickySurface.h"
#include "Components/RectLightComponent.h"

AStickySurface::AStickySurface()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AStickySurface::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> comps = GetComponentsByClass(URectLightComponent::StaticClass());
	for(auto& comp : comps)
	{
		if (URectLightComponent* rect = Cast<URectLightComponent>(comp))
		{
			const auto actorScale = GetActorScale();
			rect->SourceHeight *= actorScale.Z;
			rect->SourceWidth *= actorScale.Y;
			rect->AttenuationRadius *= FMath::Max(actorScale.Z, actorScale.Y);
		}		
	}
}
