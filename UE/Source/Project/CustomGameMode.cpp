#include "CustomGameMode.h"

#include "CustomGameInstance.h"
#include "UI/Menus/MenuManager.h"
#include "Utility/MainSingelton.h"

void ACustomGameMode::InitGameState()
{
	Super::InitGameState();

	const auto instance = UMainSingelton::GetGameInstance();
	CHECK_RETURN_LOG(!instance, "Instance nullptr");
	const auto world = GetWorld();
	CHECK_RETURN_LOG(!world, "World nullptr");
	const auto menuActor = world->SpawnActor(instance->GetMenuManagerBP().Get());
	CHECK_RETURN_LOG(!menuActor, "MenuActor nullptr");
	myMenuManager = Cast<AMenuManager>(menuActor);
	CHECK_RETURN_LOG(!menuActor, "MenuActor not of type AMenuManager");
}
