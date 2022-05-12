#include "Checkpoint.h"

#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACheckpoint::FinishActivate(AFPController* anInstigator)
{
	CHECK_RETURN_LOG(!anInstigator, "No instigator");
	anInstigator->TravelCheckpoint(this);
}
