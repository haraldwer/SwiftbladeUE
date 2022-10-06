
#include "GameDBModule.h"

IMPLEMENT_MODULE(FGameDBModule, GameDatabase);

DEFINE_LOG_CATEGORY(LogGameDB);

void FGameDBModule::StartupModule()
{
	IModuleInterface::StartupModule();
}

void FGameDBModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}