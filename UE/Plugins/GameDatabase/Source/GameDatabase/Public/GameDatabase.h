#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameDatabase.generated.h"

class UAuthentication;
class ULeaderboard;
class UBlob; 

class UNakamaClient;
class UNakamaRealtimeClient;
class UNakamaSession;

UCLASS(ClassGroup=(GameDB))
class GAMEDATABASE_API UGameDatabase : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class UDBComponent;

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void CreateClient();
	void CreateRealtimeClient(UNakamaSession* aSession);
	
	UAuthentication& GetAuthentication() const;
	ULeaderboard& GetLeaderboard() const;
	UBlob& GetBlob() const;

private:
	
	UFUNCTION()
	void OnRealtimeClientConnectSuccess();

	UFUNCTION()
	void OnRealtimeClientConnectError();

	UPROPERTY()
	TObjectPtr<UAuthentication> myAuthentication;
	UPROPERTY()
	TObjectPtr<ULeaderboard> myLeaderboard;
	UPROPERTY()
	TObjectPtr<UBlob> myBlob;

	// Nakama objects
	UPROPERTY()
	TObjectPtr<UNakamaClient> myClient;
	UPROPERTY()
	TObjectPtr<UNakamaRealtimeClient> myRealtimeClient;
	UPROPERTY()
	TObjectPtr<UNakamaSession> myUserSession;

	static constexpr TCHAR* myServerKey = TEXT("defaultkey");
	static constexpr TCHAR* myHost = TEXT("83.254.137.25");
	static constexpr TCHAR* myClientName = TEXT("Main");
	static constexpr int32 myPort = 7350;
	static constexpr float myTickInterval = 0.5f;
	static constexpr float myRealtimeTickInterval = 0.5f;
	
};
