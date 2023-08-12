#include "GameDatabase.h"

#include "NakamaClient.h"
#include "Components/Leaderboard.h"
#include "Components/Authentication.h"
#include "Components/Blob.h"
#include "GameDBModule.h"

UGameDatabase& UGameDatabase::Get()
{
	const auto world = GEngine->GetCurrentPlayWorld();
	check(world && "Invalid world");
	const auto gameInstance = world->GetGameInstance();
	check(gameInstance && "Invalid gameinstance");
	const auto subsystem = gameInstance->GetSubsystem<UGameDatabase>();
	check(subsystem && "Invalid subsystem");
	return *subsystem;
}

void UGameDatabase::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	myAuthentication = NewObject<UAuthentication>(this, "Authentication");
	myLeaderboard = NewObject<ULeaderboard>(this, "Leaderboard");
	myBlob = NewObject<UBlob>(this, "Blob");
	
	CreateClient();
}

void UGameDatabase::CreateClient()
{
	// Setup Default Client
	myClient = UNakamaClient::CreateDefaultClient(
		myServerKey,
		myHost,
		myPort,
		false,
		true,
		ENakamaClientType::DEFAULT,
		myTickInterval,
		myClientName);

	GetAuthentication().Authenticate(); 
}

void UGameDatabase::CreateRealtimeClient(UNakamaSession* aSession)
{
	check(aSession && "Invalid session");
	myUserSession = aSession;

	FOnRealtimeClientConnected success;
	success.AddDynamic(this, &UGameDatabase::OnRealtimeClientConnectSuccess);
	FOnRealtimeClientError error;
	error.AddDynamic(this, &UGameDatabase::OnRealtimeClientConnectError);

	// This is our realtime client (socket) ready to use
	myRealtimeClient = myClient->SetupRealtimeClient(
		myUserSession,
		true,
		myPort,
		ENakamaRealtimeClientProtocol::Protobuf,
		myRealtimeTickInterval, 
		aSession->SessionData.Username);

	// Remember to Connect
	myRealtimeClient->Connect(success, error);
}

UAuthentication& UGameDatabase::GetAuthentication() const
{
	const auto auth = myAuthentication.Get();
	check(auth && "Leaderboard null in AGameDB")
	return *auth;
}

ULeaderboard& UGameDatabase::GetLeaderboard() const
{
	const auto lb = myLeaderboard.Get(); 
	check(lb && "Leaderboard null in AGameDB")
	return *lb;
}

UBlob& UGameDatabase::GetBlob() const
{
	const auto blob = myBlob.Get(); 
	check(blob && "Blob null in AGameDB")
	return *blob;
}

void UGameDatabase::OnRealtimeClientConnectSuccess()
{
	UE_LOG(LogGameDB, Display, TEXT("Socket Setup Success!"));
	GetBlob().Init(); 
}

void UGameDatabase::OnRealtimeClientConnectError()
{
	UE_LOG(LogGameDB, Display, TEXT("Socket Setup Failed!"));
}
