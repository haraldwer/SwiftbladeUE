#include "Components/Authentication.h"

#include "GameDatabase.h"
#include "GameDBModule.h"
#include "JsonObjectConverter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utility/Math/Encryption.h"

void UAuthentication::Authenticate()
{
	if (LoadAuthData(myAuthData))
		Authenticate(myAuthData);
}

void UAuthentication::Authenticate(const FAuthData& someData)
{
	if (IsAuthenticated())
	{
		UE_LOG(LogGameDB, Warning, TEXT("Already authenticated"));
		if (myOnAuthError.IsBound())
			myOnAuthError.Broadcast("Already authenticated");
		return; 
	}
	
	const auto client = GetClient();
	if (!client)
	{
		UE_LOG(LogGameDB, Error, TEXT("No GameDB client on Authenticate"));
		if (myOnAuthError.IsBound())
			myOnAuthError.Broadcast("No client");
		return;
	}

	myAuthData = someData;
	
	const FString deviceID =
		UKismetSystemLibrary::GetDeviceId() + "###" +
		UKismetSystemLibrary::GetPlatformUserName() + "###" +
		someData.myToken;
	const TMap<FString, FString> variables;

	FOnAuthUpdate success;
	success.AddDynamic(this, &UAuthentication::OnAuthSuccess);
	FOnError error;
	error.AddDynamic(this, &UAuthentication::OnAuthError);

	client->AuthenticateDevice(
		deviceID,
		someData.myUsername,
		true,
		variables,
		success,
		error);
}

void UAuthentication::OnAuthSuccess(UNakamaSession* aSession)
{
	if (!aSession)
	{
		if (myOnAuthError.IsBound())
			myOnAuthError.Broadcast("Session nullptr after AuthSuccess");
		return;
	}

	UE_LOG(LogGameDB, Display, TEXT("Authenticated As %s"), *aSession->SessionData.Username);
	GetDB().CreateRealtimeClient(aSession);
	
	if (myOnAuthSuccess.IsBound())
		myOnAuthSuccess.Broadcast(aSession->IsCreated());

	SaveAuthData(myAuthData);
}

void UAuthentication::OnAuthError(const FNakamaError& aError)
{
	if (myOnAuthError.IsBound())
		myOnAuthError.Broadcast(aError.Message);
}

bool UAuthentication::LoadAuthData(FAuthData& someData)
{
	FString content;
	const FString file = FPaths::LaunchDir() + "\\" + myFileName;
	if (!FFileHelper::LoadFileToString(content, *file))
	{
		UE_LOG(LogGameDB, Warning, TEXT("Failed to load auth"));
		return false;
	}

	const FString json = Decrypt(content); 
	
	FAuthData data;
	if (!FJsonObjectConverter::JsonObjectStringToUStruct(json, &data))
	{
		UE_LOG(LogGameDB, Warning, TEXT("Failed to parse json"));
		return false;
	}
	
	return true;
}

bool UAuthentication::SaveAuthData(const FAuthData& someData)
{
	FString json;
	if (!FJsonObjectConverter::UStructToJsonObjectString(
		FAuthData::StaticStruct(),
		&someData,
		json))
	{
		UE_LOG(LogGameDB, Warning, TEXT("Failed to serialize auth"));
		return false;
	} 

	const FString encrypted = Encrypt(json);

	// We will use this FileManager to deal with the file.
	const FString file = FPaths::LaunchDir() + "\\" + myFileName;
	if (!FFileHelper::SaveStringToFile(encrypted, *file))
	{
		UE_LOG(LogGameDB, Warning, TEXT("Failed to save auth"));
		return false; 
	}

	return true; 
}
