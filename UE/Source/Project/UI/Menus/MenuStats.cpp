#include "MenuStats.h"

#include "GameDatabase.h"
#include "Components/Leaderboard.h"
#include "Project/Player/FPController.h"
#include "Project/Player/FPStateSubsystem.h"
#include "Project/Utility/MainSingelton.h"

void UMenuStats::Populate()
{
	const auto controller = UMainSingelton::GetLocalController();
	CHECK_RETURN_LOG(!controller, "Controller nullptr");

	const FFPState state = controller->GetState();
	
	FStatsData stats;
	stats.myScore = state.myTime;
	stats.myTime = state.myTime;
	stats.mySeed = state.mySeed;
	OnPopulateScore(stats); 
	
	FLeaderboardRequest request;
	request.myEntries = 10;
	request.mySeed = state.mySeed;
	request.myType = ELeaderboardType::GLOBAL;
	request.mySeedType = ELeaderboardSeed::SPECIFIC;
	
	auto& lb = UMainSingelton::GetGameDB().GetLeaderboard();
	lb.myOnListSuccess.AddUniqueDynamic(this, &UMenuStats::OnDataLoaded);
	lb.myOnListError.AddUniqueDynamic(this, &UMenuStats::UMenuStats::OnLoadingFailed);
	lb.List(request);
}

void UMenuStats::SetNextChapterState()
{
	const auto gameInstance = GetGameInstance();
	CHECK_RETURN_LOG(!gameInstance, "GameInstance nullptr")
	const auto subsystem = gameInstance->GetSubsystem<UFPStateSubsystem>();
	CHECK_RETURN_LOG(!subsystem, "FPStateSubsystem nullptr")
	auto state = subsystem->Get();
	state.myChapter++;
	subsystem->Set(state);
}
