#include "FPStateSubsystem.h"

#include "Engine/GameInstance.h"
#include "Project/Utility/GameUtility.h"

UFPStateSubsystem& UFPStateSubsystem::Get()
{
	const auto subsystem = UGameUtility::GetGameInstance().GetSubsystem<UFPStateSubsystem>();
	CHECK_ASSERT(!subsystem, "Subsystem nullptr");
	return *subsystem;
}
