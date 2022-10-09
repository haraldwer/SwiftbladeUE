#include "EnemySpawner.h"

#include "EnemyManager.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Utility/MainSingelton.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}
