#include "LevelData.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ALevelData::ALevelData()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALevelData::BeginPlay()
{
	Super::BeginPlay();
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != "Base")
		UGameplayStatics::OpenLevel(GetWorld(), FName("Base"));
}