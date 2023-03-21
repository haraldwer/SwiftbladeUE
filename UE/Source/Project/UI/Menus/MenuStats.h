#pragma once

#include "CoreMinimal.h"
#include "MenuBase.h"
#include "Data/LeaderboardData.h"
#include "MenuStats.generated.h"

USTRUCT(BlueprintType)
struct FStatsData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 myScore = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float myTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	int32 mySeed = 0.0f;
};

UCLASS()
class PROJECT_API UMenuStats : public UMenuBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void Populate(); 

	UFUNCTION(BlueprintImplementableEvent)
	void OnPopulateScore(const FStatsData& someData);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDataLoaded(const FLeaderboardData& someData);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadingFailed(const FString& anError);
	
};
