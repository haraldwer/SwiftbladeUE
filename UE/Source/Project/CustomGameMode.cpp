#include "CustomGameMode.h"

#include "CustomGameInstance.h"
#include "Enemies/EnemyManager.h"
#include "LevelGen/LevelManager.h"
#include "UI/Menus/MenuManager.h"
#include "UI/Prompts/PromptManager.h"
#include "Utility/MainSingelton.h"

void ACustomGameMode::InitGameState()
{
	Super::InitGameState();

	if (GEngine->GetCurrentPlayWorld())
	{
		CreateMenuManager();
		CreatePromptManager();
		CreateEnemyManager();
		CreateLevelGenerator();
	}
}

void ACustomGameMode::CreateMenuManager()
{
	const auto& instance = UMainSingelton::GetGameInstance();
	const auto world = GetWorld();
	CHECK_RETURN_LOG(!world, "World nullptr");
	const auto menuActor = world->SpawnActor(instance.GetMenuManagerBP().Get());
	CHECK_RETURN_LOG(!menuActor, "MenuActor nullptr");
	myMenuManager = Cast<AMenuManager>(menuActor);
	CHECK_RETURN_LOG(!menuActor, "MenuActor not of type AMenuManager");
}

void ACustomGameMode::CreatePromptManager()
{
	const auto& instance = UMainSingelton::GetGameInstance();
	const auto world = GetWorld();
	CHECK_RETURN_LOG(!world, "World nullptr");
	const auto promptActor = world->SpawnActor(instance.GetPromptManagerBP().Get());
	CHECK_RETURN_LOG(!promptActor, "PromptActor nullptr");
	myPromptManager = Cast<APromptManager>(promptActor);
	CHECK_RETURN_LOG(!promptActor, "PromptActor not of type APromptManager");
}

void ACustomGameMode::CreateEnemyManager()
{
	const auto& instance = UMainSingelton::GetGameInstance();
	const auto world = GetWorld();
	CHECK_RETURN_LOG(!world, "World nullptr");
	const auto enemyActor = world->SpawnActor(instance.GetEnemyManagerBP().Get());
	CHECK_RETURN_LOG(!enemyActor, "PromptActor nullptr");
	myEnemyManager = Cast<AEnemyManager>(enemyActor);
	CHECK_RETURN_LOG(!enemyActor, "EnemyActor not of type AEnemyManager");
}

void ACustomGameMode::CreateLevelGenerator()
{
	const auto& instance = UMainSingelton::GetGameInstance();
	const auto world = GetWorld();
	CHECK_RETURN_LOG(!world, "World nullptr");
	const auto levelGenerator = world->SpawnActor(instance.GetLevelGeneratorBP().Get());
	CHECK_RETURN_LOG(!levelGenerator, "PromptActor nullptr");
	myLevelGenerator = Cast<ALevelManager>(levelGenerator);
	CHECK_RETURN_LOG(!levelGenerator, "LevelGenerator not of type ALevelGenerator");
}
