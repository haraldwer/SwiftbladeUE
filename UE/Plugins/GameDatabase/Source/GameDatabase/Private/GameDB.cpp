#include "GameDB.h"

#include "NakamaClient.h"
#include "Leaderboard.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameDBModule.h"

AGameDB::AGameDB()
{
	PrimaryActorTick.bCanEverTick = false;
	myLeaderboard = CreateDefaultSubobject<ULeaderboard>("Leaderboard");
}

void AGameDB::BeginPlay()
{
	Super::BeginPlay();

	// Default Client Parameters
	const FString serverKey = FString(TEXT("defaultkey"));
	const FString host = FString(TEXT("localhost"));
	const FString clientName = FString(TEXT("Main"));

	// Setup Default Client
	myClient = UNakamaClient::CreateDefaultClient(serverKey, host, 7350, false, true, ENakamaClientType::DEFAULT, 0.05f, clientName);

	// Authentication Parameters
	const FString email = FString(TEXT("debug@mail.com"));
	const FString password = FString(TEXT("verysecretpassword"));
	const FString username = UKismetSystemLibrary::GetPlatformUserName();
	const TMap<FString, FString> variables;

	// Setup Delegates of same type and bind them to local functions
	FOnAuthUpdate authenticationSuccessDelegate;
	authenticationSuccessDelegate.AddDynamic(this, &AGameDB::OnAuthenticationSuccess);

	FOnError authenticationErrorDelegate;
	authenticationErrorDelegate.AddDynamic(this, &AGameDB::OnAuthenticationError);

	myClient->AuthenticateEmail(email, password, username, true, variables, authenticationSuccessDelegate, authenticationErrorDelegate);
}

ULeaderboard& AGameDB::GetLeaderboard() const
{
	check(myLeaderboard && "Leaderboard null in AGameDB")
	return *myLeaderboard;
}

void AGameDB::OnAuthenticationSuccess(UNakamaSession* aLoginData)
{
	UE_LOG(LogGameDB, Display, TEXT("Authenticated As %s"), *aLoginData->SessionData.Username);

	myUserSession = aLoginData;

	// Setup Delegates of same type and bind them to local functions
	FOnRealtimeClientConnected connectionSuccessDelegate;
	connectionSuccessDelegate.AddDynamic(this, &AGameDB::OnRealtimeClientConnectSuccess);

	FOnRealtimeClientError connectionErrorDelegate;
	connectionErrorDelegate.AddDynamic(this, &AGameDB::OnRealtimeClientConnectError);

	// This is our realtime client (socket) ready to use
	myRealtimeClient = myClient->SetupRealtimeClient(
		myUserSession,
		true,
		7350,
		ENakamaRealtimeClientProtocol::Protobuf,
		0.5f, 
		"");

	// Remember to Connect
	myRealtimeClient->Connect(connectionSuccessDelegate, connectionErrorDelegate);
}

void AGameDB::OnAuthenticationError(const FNakamaError& aError)
{
	UE_LOG(LogGameDB, Display, TEXT("Failed to Authenticate: %s"), *aError.Message);
}

void AGameDB::OnRealtimeClientConnectSuccess()
{
	UE_LOG(LogGameDB, Display, TEXT("Socket Setup Success!"));
}

void AGameDB::OnRealtimeClientConnectError()
{
	UE_LOG(LogGameDB, Display, TEXT("Socket Setup Failed!"));
}
