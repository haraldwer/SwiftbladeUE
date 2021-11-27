#include "MainSingelton.h"

#include "Kismet/GameplayStatics.h"
#include "Project/CustomGameInstance.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"
#include "Project/CustomGameMode.h"
#include "Project/UI/Menus/MenuManager.h"

AMenuManager* UMainSingelton::GetMenuManger()
{
	const auto gameMode = GetGameMode();
	CHECK_ASSERT(!gameMode, "GameMode nullptr");
	const auto menuManager = Cast<AMenuManager>(gameMode->myMenuManager);
	CHECK_ASSERT(!menuManager, "MenuManager nullptr");
	return menuManager;
}

AFPController* UMainSingelton::GetLocalController()
{
	const auto instance = GetGameInstance();
	return Cast<AFPController>(instance->GetPrimaryPlayerController());
}

AFPCharacter* UMainSingelton::GetLocalPlayer()
{
	const auto controller = GetLocalController();
	CHECK_RETURN(!controller, nullptr);
	return Cast<AFPCharacter>(controller->GetCharacter());
}

ACustomGameMode* UMainSingelton::GetGameMode()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	CHECK_ASSERT(!world, "World nullptr");
	const auto instance = UGameplayStatics::GetGameMode(world);
	CHECK_ASSERT(!instance, "GameMode nullptr");
	const auto customInstance = Cast<ACustomGameMode>(instance);
	CHECK_ASSERT(!customInstance, "Custom GameMode nullptr");
	return customInstance;
}

UCustomGameInstance* UMainSingelton::GetGameInstance()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	CHECK_ASSERT(!world, "World nullptr");
	const auto instance = UGameplayStatics::GetGameInstance(world);
	CHECK_ASSERT(!instance, "Instance nullptr");
	const auto customInstance = Cast<UCustomGameInstance>(instance);
	CHECK_ASSERT(!customInstance, "Custom instance nullptr");
	return customInstance;
}
