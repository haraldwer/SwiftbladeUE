#include "MenuStats.h"

#include "GameDatabase.h"
#include "Components/Leaderboard.h"
#include "Project/Player/FPController.h"
#include "Project/Player/FPStateSubsystem.h"
#include "Project/Utility/GameUtility.h"

void UMenuStats::Populate()
{
	const auto controller = UGameUtility::GetLocalController();
	CHECK_RETURN_LOG(!controller, "Controller nullptr");

	const FFPState state = UFPStateSubsystem::Get().GetState(); 
	
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
	
	auto& lb = UGameDatabase::Get().GetLeaderboard();
	lb.myOnListSuccess.AddUniqueDynamic(this, &UMenuStats::OnDataLoaded);
	lb.myOnListError.AddUniqueDynamic(this, &UMenuStats::UMenuStats::OnLoadingFailed);
	lb.List(request);
}

void UMenuStats::SetNextChapterState()
{
	auto& subsystem = UFPStateSubsystem::Get(); 
	auto state = subsystem.GetState();
	state.myChapter++;
	subsystem.SetState(state);
}
