#include "GameUtility.h"

#include "Components/Authentication.h"
#include "GameDatabase.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"

bool UGameUtility::IsInBaseLevel()
{
	CHECK_RETURN(!GEngine, false);
	const UWorld* world = GEngine->GetCurrentPlayWorld();
	CHECK_RETURN(!world, false);
	return UGameplayStatics::GetCurrentLevelName(world) == "ML_Base";
}

bool UGameUtility::IsAuthenticated()
{
	const auto gameDB = GetGameInstance().GetSubsystem<UGameDatabase>();
	CHECK_RETURN(!gameDB, false);
	return gameDB->GetAuthentication().IsAuthenticated(); 
}

AFPController* UGameUtility::GetLocalController()
{
	const auto& instance = GetGameInstance();
	return Cast<AFPController>(instance.GetFirstLocalPlayerController());
}

AFPCharacter* UGameUtility::GetLocalPlayer()
{
	const auto controller = GetLocalController();
	CHECK_RETURN(!controller, nullptr);
	return Cast<AFPCharacter>(controller->GetCharacter());
}

AGameModeBase& UGameUtility::GetGameMode()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	CHECK_ASSERT(!world, "World nullptr");
	const auto instance = UGameplayStatics::GetGameMode(world);
	CHECK_ASSERT(!instance, "GameMode nullptr");
	return *instance;
}

UGameInstance& UGameUtility::GetGameInstance()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	CHECK_ASSERT(!world, "World nullptr");
	const auto instance = UGameplayStatics::GetGameInstance(world);
	CHECK_ASSERT(!instance, "Instance nullptr");
	return *instance;
}

UWorld& UGameUtility::GetCurrentWorld()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	CHECK_ASSERT(!world, "World nullptr");
	return *world; 
}