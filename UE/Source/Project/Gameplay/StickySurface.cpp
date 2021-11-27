// Fill out your copyright notice in the Description page of Project Settings.


#include "StickySurface.h"

#include "Components/RectLightComponent.h"


// Sets default values
AStickySurface::AStickySurface()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
