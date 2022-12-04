#include "MenuLeaderboard.h"

#include "GameDB.h"
#include "Leaderboard.h"
#include "Project/Utility/MainSingelton.h"

void UMenuLeaderboard::FetchData(ELeaderboardType aType, ELeaderboardSeed aSeed)
{
	auto& lb = UMainSingelton::GetGameDB().GetLeaderboard();
	lb.myOnListSuccess.AddDynamic(this, &UMenuLeaderboard::OnDataLoaded);
	lb.myOnListError.AddDynamic(this, &UMenuLeaderboard::OnLoadingFailed);
	lb.List("Global");
}
