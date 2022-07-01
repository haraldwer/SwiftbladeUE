#include "LevelData.h"

#include "Engine/World.h"
#include "Project/Player/FPController.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Utility/MainSingelton.h"

ALevelData::ALevelData()
{
	PrimaryActorTick.bCanEverTick = false;
}