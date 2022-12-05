#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LeaderboardData.h"
#include "NakamaClient.h"

#include "Leaderboard.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEDATABASE_API ULeaderboard : public UActorComponent
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnListSuccess, const FLeaderboardData&, someData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWriteSuccess);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeaderboardError, const FString&, anError);
	
	ULeaderboard();
	
	void List(const FLeaderboardRequest& aRequest) const;
	FOnListSuccess myOnListSuccess;
	FOnLeaderboardError myOnListError;
	
	void Write(const FLeaderboardSubmission& aSubmission) const;
	FOnWriteSuccess myOnWriteSuccess;
	FOnLeaderboardError myOnWriteError;

private:
	
	class AGameDB& GetDB() const;
	
	UFUNCTION()
	void OnListResult(const FNakamaRPC& aResult);
	UFUNCTION()
	void OnListError(const FNakamaError& anError); 

	UFUNCTION()
	void OnWriteResult(const FNakamaRPC& aResult);
	UFUNCTION()
	void OnWriteError(const FNakamaError& anError); 

	static constexpr float myTimePrecision = 1000.0f;
	
};
