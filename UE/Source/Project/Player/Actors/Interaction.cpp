#include "Interaction.h"
#include "Components/SphereComponent.h"

AInteraction::AInteraction()
{
	PrimaryActorTick.bCanEverTick = false;
	myCollider = CreateDefaultSubobject<USphereComponent>("Collider");
	myCollider->SetupAttachment(RootComponent);
}

void AInteraction::BeginPlay()
{
	Super::BeginPlay();
	
}