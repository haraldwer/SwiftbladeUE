#include "Leaderboard.h"
#include "GameDB.h"
#include "GameDBModule.h"
#include "JsonObjectConverter.h"

ULeaderboard::ULeaderboard()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AGameDB& ULeaderboard::GetDB() const
{
	const auto ptr = Cast<AGameDB>(GetOwner());
	check(ptr && "Missing valid gameDB owner in ULeaderboard")
	return *ptr; 	
}

// - List - // 

void ULeaderboard::List(const FLeaderboardRequest& aRequest) const
{
	FString json;
	FJsonObjectConverter::UStructToJsonObjectString(
		FLeaderboardRequest::StaticStruct(),
		&aRequest,
		json);
	
	FOnRPC OnListSuccess;
	OnListSuccess.AddDynamic(this, &ULeaderboard::OnListResult);
	FOnError OnListError;
	OnListError.AddDynamic(this, &ULeaderboard::OnListError);

	const auto& db = GetDB();
	db.myClient->RPC(
		db.myUserSession, 
		"ListLeaderboard_Lua",
		json,
		OnListSuccess,
		OnListError);

	UE_LOG(LogGameDB, Display, TEXT("Listing leaderboard: %s"), *json);
}

void ULeaderboard::OnListResult(const FNakamaRPC& aResult)
{
	UE_LOG(LogGameDB, Display, TEXT("Leaderboard received"));

	// {"payload":{"
	// 		myEntries":[{
	// 			"myName":"haral",
	// 			"myRank":1,
	// 			"myScore":11113,
	// 			"myUserID":"6f2faca1-b9a7-4967-8bd3-e136f9d94a21"
	// 		}],
	// 		"mySeed":"-1",
	// 		"mySeedType":"ANY",
	// 		"myType":"PERSONAL"
	// 	},
	// "success":true}
	
	FJsonObjectWrapper json;
	if (!json.JsonObjectFromString(aResult.Payload))
	{
		UE_LOG(LogGameDB, Error, TEXT("Failed to parse Json: %s"), *aResult.Payload);
		return; 
	}
	const auto jsonObj = json.JsonObject.Get();
	if (!jsonObj)
		return; 
	
	const auto payloadObj = jsonObj->GetObjectField("payload");
	if (!payloadObj)
		return;
	
	FLeaderboardData data;
	FJsonObjectConverter::JsonObjectToUStruct(payloadObj.ToSharedRef(), &data);
	myOnListSuccess.Broadcast(data);
}

void ULeaderboard::OnListError(const FNakamaError& anError)
{
	UE_LOG(LogGameDB, Display, TEXT("Failed to list leaderboard: %s"), *anError.Message);
	myOnListError.Broadcast(anError.Message);
}

// - Write - // 

void ULeaderboard::Write(const FLeaderboardSubmission& aSubmission) const
{
	FString json;
	FJsonObjectConverter::UStructToJsonObjectString(
		FLeaderboardSubmission::StaticStruct(),
		&aSubmission,
		json);
	
	FOnRPC OnWriteSuccess;
	OnWriteSuccess.AddDynamic(this, &ULeaderboard::OnWriteResult);
	FOnError OnWriteError;
	OnWriteError.AddDynamic(this, &ULeaderboard::OnWriteError);
	
	const auto& db = GetDB();
	db.myClient->RPC(
		db.myUserSession, 
		"WriteLeaderboard_Lua",
		json,
		OnWriteSuccess,
		OnWriteError);

	UE_LOG(LogGameDB, Display, TEXT("Submitting score to leaderboard: %f, %f"),
		aSubmission.myTime, static_cast<float>(aSubmission.mySeed));
}

void ULeaderboard::OnWriteResult(const FNakamaRPC& aResult)
{
	UE_LOG(LogGameDB, Display, TEXT("Score submitted: %s"), *aResult.Payload);
	myOnWriteSuccess.Broadcast();
}

void ULeaderboard::OnWriteError(const FNakamaError& anError)
{
	UE_LOG(LogGameDB, Display, TEXT("Failed to create leaderboard: %s"), *anError.Message);
	myOnWriteError.Broadcast(anError.Message);
}