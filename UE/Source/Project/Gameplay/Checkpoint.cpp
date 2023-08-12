#include "Checkpoint.h"

#include "../Enemies/EnemySubsystem.h"
#include "Project/Player/FPController.h"
#include "Project/Utility/GameUtility.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (UGameUtility::IsInBaseLevel())
		UEnemySubsystem::Get().myOnEnemiesCleared.AddDynamic(this, &ACheckpoint::Enable);
	else
		Enable();
}

void ACheckpoint::Enable()
{
	myEnabled = true;
	OnEnable();
}

void ACheckpoint::FinishActivate(AFPController* anInstigator)
{
	CHECK_RETURN_LOG(!anInstigator, "No instigator");
	anInstigator->UseCheckpoint(*this);
}
