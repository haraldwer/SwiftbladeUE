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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreateSuccess);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnListSuccess, const FLeaderboardData&, someData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWriteSuccess);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeaderboardError, const FString&, anError);
	
	ULeaderboard();
	
	void Create(const FString& aLeaderboardID) const;
	FOnCreateSuccess myOnCreateSuccess;
	FOnLeaderboardError myOnCreateError;
	
	void List(const FString& aLeaderboardID) const;
	FOnListSuccess myOnListSuccess;
	FOnLeaderboardError myOnListError;
	
	void Write(const FString& aLeaderboardID, float aScore) const;
	FOnWriteSuccess myOnWriteSuccess;
	FOnLeaderboardError myOnWriteError;

private:
	
	class AGameDB& GetDB() const;

	UFUNCTION()
	void OnCreateResult(const FNakamaRPC& aResult);
	UFUNCTION()
	void OnCreateError(const FNakamaError& anError);
	
	UFUNCTION()
	void OnListResult(const FNakamaLeaderboardRecordList& aResult);
	UFUNCTION()
	void OnListError(const FNakamaError& anError); 
	
	UFUNCTION()
	void OnWriteResult(const FNakamaLeaderboardRecord& aResult);
	UFUNCTION()
	void OnWriteError(const FNakamaError& anError); 

	static constexpr float myTimePrecision = 1000.0f;  
	
};
