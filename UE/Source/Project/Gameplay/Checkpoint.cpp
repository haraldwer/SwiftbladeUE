#include "Checkpoint.h"

#include "Project/Enemies/EnemyManager.h"
#include "Project/Player/FPController.h"
#include "Project/Utility/GameUtility.h"
#include "Project/Utility/MainSingelton.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (UGameUtility::IsInBaseLevel())
		UMainSingelton::GetEnemyManager().myOnEnemiesCleared.AddDynamic(this, &ACheckpoint::Enable);
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
	anInstigator->UseCheckpoint(this);
}
