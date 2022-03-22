#include "Door.h"

#include "Project/Player/FPCharacter.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADoor::Open(AFPCharacter* anInstigator)
{
	myInstigator = anInstigator;
	OnOpen();
}

void ADoor::NotifyOpen()
{
	OnOpened();
	myInstigator->DoorOpened(this);
}
