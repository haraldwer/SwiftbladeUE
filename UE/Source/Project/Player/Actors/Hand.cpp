#include "Hand.h"

#include "Components/StaticMeshComponent.h"

AHand::AHand()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHand::SetHandState(const EHandState aState) const
{
	TArray<UStaticMeshComponent*> meshes;
	GetComponents<UStaticMeshComponent>(meshes);
	for (int32 i = 0; i < meshes.Num(); i++)
		meshes[i]->SetVisibility(i == static_cast<int32>(aState));
}
