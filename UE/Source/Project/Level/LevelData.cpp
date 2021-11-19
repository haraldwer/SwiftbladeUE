#include "LevelData.h"

#include "GameMapsSettings.h"
#include "Project/Utility.h"

ALevelData::ALevelData()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALevelData::BeginPlay()
{
	Super::BeginPlay();
	const auto defaultLevel = UGameMapsSettings::GetGameDefaultMap();
	if (GetWorld()->GetCurrentLevel() != defaultLevel)
	{
		LOG("Starting from sublevel, will load StartLevel instead");
		UGameplayStatics::OpenLevel(GetWorld(), FName(*defaultLevel));
	}
}