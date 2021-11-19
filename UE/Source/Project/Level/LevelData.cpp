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
	
	LOG(UGameplayStatics::GetCurrentLevelName(GetWorld()));
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != "Base")
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("Base"));
	}
}