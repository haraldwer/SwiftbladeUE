#pragma once

#include "MenuBase.h"
#include "LeaderboardData.h"
#include "MenuLeaderboard.generated.h"

UCLASS(Blueprintable)
class PROJECT_API UMenuLeaderboard : public UMenuBase
{
	GENERATED_BODY()

public:

	virtual ~UMenuLeaderboard() override = default;  
	
	UFUNCTION(BlueprintCallable)
	void FetchData();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDataLoaded(const FLeaderboardData& someData);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadingFailed(const FString& anError);

};
