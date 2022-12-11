#include "GameUtility.h"

#include "Components/Authentication.h"
#include "GameDatabase.h"
#include "MainSingelton.h"
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
