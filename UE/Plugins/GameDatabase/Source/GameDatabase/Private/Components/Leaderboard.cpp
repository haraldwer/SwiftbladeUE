#include "Components/Leaderboard.h"

#include "GameDBModule.h"
#include "JsonObjectConverter.h"

// - List - // 

void ULeaderboard::List(const FLeaderboardRequest& aRequest) const
{
	const auto client = GetClient();
	const auto session = GetSession();
	if (!client || !session)
	{
		UE_LOG(LogGameDB, Error, TEXT("GameDB not yet connected on lb list"));
		myOnListError.Broadcast("GameDB not yet connected on lb list"); 
		return;
	}
	
	FString json;
	FJsonObjectConverter::UStructToJsonObjectString(
		FLeaderboardRequest::StaticStruct(),
		&aRequest,
		json);
	
	FOnRPC success;
	success.AddUniqueDynamic(this, &ULeaderboard::OnListResult);
	FOnError error;
	error.AddUniqueDynamic(this, &ULeaderboard::OnListError);

	client->RPC(
		session, 
		"Lua_LB_List",
		json,
		success,
		error);

	UE_LOG(LogGameDB, Display, TEXT("Listing leaderboard: %s"), *json);
}

void ULeaderboard::OnListResult(const FNakamaRPC& aResult)
{
	UE_LOG(LogGameDB, Display, TEXT("Leaderboard received: %s"), *aResult.Payload);

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
	UE_LOG(LogGameDB, Error, TEXT("Failed to list leaderboard: %s"), *anError.Message);
	myOnListError.Broadcast(anError.Message);
}

// - Write - // 

void ULeaderboard::Write(const FLeaderboardSubmission& aSubmission) const
{
	const auto client = GetClient();
	const auto session = GetSession();
	if (!client || !session)
	{
		UE_LOG(LogGameDB, Error, TEXT("GameDB not yet connected on lb write"));
		myOnWriteError.Broadcast("GameDB not yet connected on lb write"); 
		return;
	}
	
	FString json;
	FJsonObjectConverter::UStructToJsonObjectString(
		FLeaderboardSubmission::StaticStruct(),
		&aSubmission,
		json);
	
	FOnRPC success;
	success.AddUniqueDynamic(this, &ULeaderboard::OnWriteResult);
	FOnError error;
	error.AddUniqueDynamic(this, &ULeaderboard::OnWriteError);
	
	client->RPC(
		session, 
		"Lua_LB_Write",
		json,
		success,
		error);

	UE_LOG(LogGameDB, Display, TEXT("Submitting score to leaderboard: %f, %f"),
		aSubmission.myScore, static_cast<float>(aSubmission.mySeed));
}

void ULeaderboard::OnWriteResult(const FNakamaRPC& aResult)
{
	UE_LOG(LogGameDB, Display, TEXT("Score submitted: %s"), *aResult.Payload);
	myOnWriteSuccess.Broadcast();
}

void ULeaderboard::OnWriteError(const FNakamaError& anError)
{
	UE_LOG(LogGameDB, Error, TEXT("Failed to write to leaderboard: %s"), *anError.Message);
	myOnWriteError.Broadcast(anError.Message);
}