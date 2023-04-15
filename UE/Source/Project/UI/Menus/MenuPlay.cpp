#include "MenuPlay.h"

#include "Project/Player/FPStateSubsystem.h"

void UMenuPlay::SetState(const int32 aChapter, const int32 aSeed)
{
	const auto gameInstance = GetGameInstance();
	CHECK_RETURN_LOG(!gameInstance, "GameInstance nullptr")
	const auto subsystem = gameInstance->GetSubsystem<UFPStateSubsystem>();
	CHECK_RETURN_LOG(!subsystem, "FPStateSubsystem nullptr")
	auto state = subsystem->Get();
	state.myChapter = aChapter;
	state.mySeed = aSeed;
	subsystem->Set(state);
}