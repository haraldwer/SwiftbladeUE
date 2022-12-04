#include "MainSingelton.h"

#include "Kismet/GameplayStatics.h"
#include "Project/CustomGameInstance.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"
#include "Project/CustomGameMode.h"
#include "Project/LevelGen/Level/LevelManager.h"

AMenuManager* UMainSingelton::GetMenuMangerPtr()
{
	const auto& gameMode = GetGameMode();
	CHECK_ASSERT(!gameMode.myMenuManager, "MenuManager nullptr");
	return gameMode.myMenuManager;
}

AMenuManager& UMainSingelton::GetMenuManger()
{
	return *GetMenuMangerPtr();
}

APromptManager* UMainSingelton::GetPromptManagerPtr()
{
	const auto& gameMode = GetGameMode();
	CHECK_ASSERT(!gameMode.myPromptManager, "PromptManager nullptr");
	return gameMode.myPromptManager;
}

APromptManager& UMainSingelton::GetPromptManager()
{
	return *GetPromptManagerPtr();
}

AEnemyManager* UMainSingelton::GetEnemyManagerPtr()
{
	const auto& gameMode = GetGameMode();
	CHECK_ASSERT(!gameMode.myEnemyManager, "EnemyManager nullptr");
	return gameMode.myEnemyManager;
}

AEnemyManager& UMainSingelton::GetEnemyManager()
{
	return *GetEnemyManagerPtr();
}

ALevelManager* UMainSingelton::GetLevelManagerPtr()
{
	const auto& gameMode = GetGameMode();
	CHECK_ASSERT(!gameMode.myLevelGenerator, "LevelGenerator nullptr");
	return gameMode.myLevelGenerator;
}

ALevelManager& UMainSingelton::GetLevelManager()
{
	return *GetLevelManagerPtr();
}

AGameDB* UMainSingelton::GetGameDBPtr()
{
	const auto& gameMode = GetGameMode();
	CHECK_ASSERT(!gameMode.myGameDB, "EnemyManager nullptr");
	return gameMode.myGameDB; 
}

AGameDB& UMainSingelton::GetGameDB()
{
	return *GetGameDBPtr(); 
}

AFPController* UMainSingelton::GetLocalController()
{
	const auto& instance = GetGameInstance();
	return Cast<AFPController>(instance.GetFirstLocalPlayerController());
}

AFPCharacter* UMainSingelton::GetLocalPlayer()
{
	const auto controller = GetLocalController();
	CHECK_RETURN(!controller, nullptr);
	return Cast<AFPCharacter>(controller->GetCharacter());
}

ACustomGameMode& UMainSingelton::GetGameMode()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	CHECK_ASSERT(!world, "World nullptr");
	const auto instance = UGameplayStatics::GetGameMode(world);
	CHECK_ASSERT(!instance, "GameMode nullptr");
	const auto customInstance = Cast<ACustomGameMode>(instance);
	CHECK_ASSERT(!customInstance, "Custom GameMode nullptr");
	return *customInstance;
}

UCustomGameInstance& UMainSingelton::GetGameInstance()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	CHECK_ASSERT(!world, "World nullptr");
	const auto instance = UGameplayStatics::GetGameInstance(world);
	CHECK_ASSERT(!instance, "Instance nullptr");
	const auto customInstance = Cast<UCustomGameInstance>(instance);
	CHECK_ASSERT(!customInstance, "Custom instance nullptr");
	return *customInstance;
}
