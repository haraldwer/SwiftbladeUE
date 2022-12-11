#include "MenuLeaderboard.h"

#include "GameDatabase.h"
#include "Components/Leaderboard.h"
#include "Project/Utility/MainSingelton.h"

void UMenuLeaderboard::FetchData(bool aForce)
{
	if (myRequest == myLastRequest && !aForce)
		return;
	myLastRequest = myRequest;
	
	auto& lb = UMainSingelton::GetGameDB().GetLeaderboard();
	lb.myOnListSuccess.AddUniqueDynamic(this, &UMenuLeaderboard::OnDataLoaded);
	lb.myOnListError.AddUniqueDynamic(this, &UMenuLeaderboard::OnLoadingFailed);
	lb.List(myRequest);
}
