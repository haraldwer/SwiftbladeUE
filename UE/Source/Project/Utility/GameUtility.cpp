#include "GameUtility.h"

#include "Components/Authentication.h"
#include "GameDatabase.h"
#include "Kismet/GameplayStatics.h"

bool UGameUtility::IsInBaseLevel()
{
	CHECK_RETURN(!GEngine, false);
	const UWorld* world = GEngine->GetCurrentPlayWorld();
	CHECK_RETURN(!world, false);
	return UGameplayStatics::GetCurrentLevelName(world) == "ML_Base";
}

bool UGameUtility::IsAuthenticated()
{
	return UMainSingelton::GetGameDB().GetAuthentication().IsAuthenticated(); 
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

ACustomGameMode& UGameUtility::GetGameMode()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	CHECK_ASSERT(!world, "World nullptr");
	const auto instance = UGameplayStatics::GetGameMode(world);
	CHECK_ASSERT(!instance, "GameMode nullptr");
	const auto customInstance = Cast<ACustomGameMode>(instance);
	CHECK_ASSERT(!customInstance, "Custom GameMode nullptr");
	return *customInstance;
}

UCustomGameInstance& UGameUtility::GetGameInstance()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	CHECK_ASSERT(!world, "World nullptr");
	const auto instance = UGameplayStatics::GetGameInstance(world);
	CHECK_ASSERT(!instance, "Instance nullptr");
	const auto customInstance = Cast<UCustomGameInstance>(instance);
	CHECK_ASSERT(!customInstance, "Custom instance nullptr");
	return *customInstance;
}