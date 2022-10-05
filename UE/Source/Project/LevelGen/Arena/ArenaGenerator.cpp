#include "ArenaGenerator.h"
#include "ArenaConfig.h"

void AArenaGenerator::Generate()
{
	Super::Generate(); 

	// Select random config
	UArenaConfig* config = GetRandomConfig();
	if (!config)
		return;

	
}

UArenaConfig* AArenaGenerator::GetRandomConfig() const
{
	CHECK_RETURN_LOG(!myConfigs.Num(), "No configs", nullptr);
	const int32 index = FMath::RandRange(0, myConfigs.Num() - 1);
	const auto configClass = myConfigs[index];
	const auto config = configClass.GetDefaultObject();
	CHECK_RETURN_LOG(!config, "No config for type", nullptr);
	return config;
}
