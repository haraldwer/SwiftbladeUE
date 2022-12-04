#include "Leaderboard.h"
#include "GameDB.h"
#include "GameDBModule.h"

ULeaderboard::ULeaderboard()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULeaderboard::List(const FLeaderboardRequest& aRequest) const
{
	FOnListLeaderboardRecords OnListSuccess;
	OnListSuccess.AddDynamic(this, &ULeaderboard::OnListResult);
	FOnError OnListError;
	OnListError.AddDynamic(this, &ULeaderboard::OnListError);
	
	const auto& db = GetDB();
	db.myClient->ListLeaderboardRecords(
		db.myUserSession,
		aLeaderboardID,
		{},
		10,
		FString(),
		ENakamaLeaderboardListBy::BY_SCORE,
		OnListSuccess,
		OnListError);

	UE_LOG(LogGameDB, Display, TEXT("Listing leaderboard: %s"), *aLeaderboardID);
}

void ULeaderboard::OnListResult(const FNakamaLeaderboardRecordList& aResult)
{
	UE_LOG(LogGameDB, Display, TEXT("Leaderboard received"));

	FLeaderboardData data;
	for (auto& record : aResult.Records)
		data.myEntries.Add({
			record.Username,
			record.Score / myTimePrecision,
			0});
	
	myOnListSuccess.Broadcast(data);
}

void ULeaderboard::OnListError(const FNakamaError& anError)
{
	UE_LOG(LogGameDB, Display, TEXT("Failed to list leaderboard: %s"), *anError.Message);
	myOnListError.Broadcast(anError.Message);
}

void ULeaderboard::Write(const FLeaderboardSubmission& aSubmission) const
{
	FOnWriteLeaderboardRecord OnWriteSuccess;
	OnWriteSuccess.AddDynamic(this, &ULeaderboard::OnWriteResult);
	FOnError OnWriteError;
	OnWriteError.AddDynamic(this, &ULeaderboard::OnWriteError);

	// Payload
	aLeaderboardID,
	aScore * myTimePrecision,
	
	const auto& db = GetDB();
	db.myClient->RPC(
		db.myUserSession, 
		"WriteLeaderboard_Lua",
		json,
		OnWriteSuccess,
		OnWriteError);

	UE_LOG(LogGameDB, Display, TEXT("Submitting score %f to leaderboard: %s"), static_cast<float>(aScore), *aLeaderboardID);
}

void ULeaderboard::OnWriteResult(const FNakamaLeaderboardRecord& aResult)
{
	UE_LOG(LogGameDB, Display, TEXT("Score submitted: %f %s"), static_cast<float>(aResult.Score), *aResult.LeaderboardId);
	myOnWriteSuccess.Broadcast();
}

void ULeaderboard::OnWriteError(const FNakamaError& anError)
{
	UE_LOG(LogGameDB, Display, TEXT("Failed to create leaderboard: %s"), *anError.Message);
	myOnWriteError.Broadcast(anError.Message);
}

AGameDB& ULeaderboard::GetDB() const
{
	const auto ptr = Cast<AGameDB>(GetOwner());
	check(ptr && "Missing valid gameDB owner in ULeaderboard")
	return *ptr; 	
}
