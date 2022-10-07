#include "EngineUtility.h"
#include "Kismet/GameplayStatics.h"

bool UEngineUtility::IsShipping()
{
#ifdef UE_BUILD_SHIPPING
	return true;
#endif
	return false;
}

bool UEngineUtility::IsInBaseLevel()
{
	CHECK_RETURN(!GEngine, false);
	const UWorld* world = GEngine->GetCurrentPlayWorld();
	CHECK_RETURN(!world, false);
	return UGameplayStatics::GetCurrentLevelName(world) == "ML_Base";
}
