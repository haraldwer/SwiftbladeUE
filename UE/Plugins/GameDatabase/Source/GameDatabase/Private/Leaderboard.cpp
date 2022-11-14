#include "Leaderboard.h"
#include "GameDB.h"
#include "GameDBModule.h"

ULeaderboard::ULeaderboard()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULeaderboard::Create(const FString& aLeaderboardID) const
{
	const FString json = "{ \"LeaderboardID\" : \"" + aLeaderboardID + "\" }";

	FOnRPC OnCreateSuccces;
	OnCreateSuccces.AddDynamic(this, &ULeaderboard::OnCreateResult);
	FOnError OnCreateError;
	OnCreateError.AddDynamic(this, &ULeaderboard::OnCreateError);
	
	const auto& db = GetDB();
	db.myClient->RPC(
		db.myUserSession, 
		"CreateLeaderboard_Lua",
		json,
		OnCreateSuccces,
		OnCreateError);

	UE_LOG(LogGameDB, Display, TEXT("Creating leaderboard: %s"), *aLeaderboardID);
}

void ULeaderboard::OnCreateResult(const FNakamaRPC& aResult)
{
	UE_LOG(LogGameDB, Display, TEXT("Leaderboard created with result: %s"), *aResult.Payload);
	myOnCreateSuccess.Broadcast();
}

void ULeaderboard::OnCreateError(const FNakamaError& anError)
{
	UE_LOG(LogGameDB, Display, TEXT("Failed to create leaderboard: %s"), *anError.Message);
	myOnCreateError.Broadcast(anError.Message);
}

void ULeaderboard::List(const FString& aLeaderboardID) const
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
			static_cast<float>(record.Score),
			0});
	
	myOnListSuccess.Broadcast(data);
}

void ULeaderboard::OnListError(const FNakamaError& anError)
{
	UE_LOG(LogGameDB, Display, TEXT("Failed to list leaderboard: %s"), *anError.Message);
	myOnListError.Broadcast(anError.Message);
}

void ULeaderboard::Write(const FString& aLeaderboardID, int64 aScore) const
{
	// TODO: Maybe write custom function and do this in one call?
	// Or wait for create result?
	Create(aLeaderboardID);

	FOnWriteLeaderboardRecord OnWriteSuccess;
	OnWriteSuccess.AddDynamic(this, &ULeaderboard::OnWriteResult);
	FOnError OnWriteError;
	OnWriteError.AddDynamic(this, &ULeaderboard::OnWriteError);
	
	const int64 subScore = 0;
	const FString meta = "";
	const auto& db = GetDB();
	db.myClient->WriteLeaderboardRecord(
		db.myUserSession,
		aLeaderboardID,
		aScore,
		subScore,
		meta,
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
