#include "Hand.h"

#include "Components/StaticMeshComponent.h"

AHand::AHand()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHand::SetIsOpen(bool aIsOpen) const
{
	TArray<UStaticMeshComponent*> meshes;
	GetComponents<UStaticMeshComponent>(meshes);
	CHECK_RETURN(meshes.Num() < 2);
	meshes[0]->SetVisibility(aIsOpen);
	meshes[1]->SetVisibility(!aIsOpen);
}
