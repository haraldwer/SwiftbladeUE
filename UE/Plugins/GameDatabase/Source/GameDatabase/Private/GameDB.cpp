#include "GameDB.h"

AGameDB::AGameDB()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGameDB::BeginPlay()
{
	Super::BeginPlay();

	// Default Client Parameters
	const FString serverKey = FString(TEXT("defaultkey"));
	const FString host = FString(TEXT("localhost"));
	const FString clientName = FString(TEXT("Main"));

	// Setup Default Client
	NakamaClient = UNakamaClient::CreateDefaultClient(serverKey, host, 7350, false, true, ENakamaClientType::DEFAULT, 0.05f, clientName);

	// Authentication Parameters
	const FString email = FString(TEXT("debug@mail.com"));
	const FString password = FString(TEXT("verysecretpassword"));
	const FString username = FString(TEXT("debug-user"));
	TMap<FString, FString> variables;

	// Setup Delegates of same type and bind them to local functions
	FOnAuthUpdate authenticationSuccessDelegate;
	authenticationSuccessDelegate.AddDynamic(this, &AGameDB::OnAuthenticationSuccess);

	FOnError authenticationErrorDelegate;
	authenticationErrorDelegate.AddDynamic(this, &AGameDB::OnAuthenticationError);

	NakamaClient->AuthenticateEmail(email, password, username, true, variables, authenticationSuccessDelegate, authenticationErrorDelegate);
}

void AGameDB::OnAuthenticationSuccess(UNakamaSession* aLoginData)
{
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Authenticated As %s"), *aLoginData->SessionData.Username));

	UserSession = aLoginData;

	// Setup Delegates of same type and bind them to local functions
	FOnRealtimeClientConnected connectionSuccessDelegate;
	connectionSuccessDelegate.AddDynamic(this, &AGameDB::OnRealtimeClientConnectSuccess);

	FOnRealtimeClientError connectionErrorDelegate;
	connectionErrorDelegate.AddDynamic(this, &AGameDB::OnRealtimeClientConnectError);

	// This is our realtime client (socket) ready to use
	NakamaRealtimeClient = NakamaClient->SetupRealtimeClient(UserSession, true, 7350, ENakamaRealtimeClientProtocol::Protobuf, 0.05f, "");

	// Remember to Connect
	NakamaRealtimeClient->Connect(connectionSuccessDelegate, connectionErrorDelegate);
}

void AGameDB::OnAuthenticationError(const FNakamaError& aError)
{
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Failed to Authenticate: %s"), *aError.Message));
}

void AGameDB::OnRealtimeClientConnectSuccess()
{
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString(TEXT("Socket Setup Success!")));

	// Example of Joining Chat without callbacks
	NakamaRealtimeClient->JoinChat("Heroes", ENakamaChannelType::ROOM, true, false, {}, {});
}

void AGameDB::OnRealtimeClientConnectError()
{
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString(TEXT("Socket Setup Failed!")));
}
