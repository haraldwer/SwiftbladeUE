#include "MenuPlay.h"

#include "Project/Player/FPStateSubsystem.h"

void UMenuPlay::SetState(const int32 aChapter, const int32 aSeed)
{
	auto& subsystem = UFPStateSubsystem::Get();
	auto state = subsystem.GetState();
	state.myChapter = aChapter;
	state.mySeed = aSeed;
	subsystem.SetState(state);
}