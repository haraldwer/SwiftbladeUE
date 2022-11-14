#pragma once

#include "CoreMinimal.h"
#include "NakamaError.h" // TODO: Fix include
#include "GameDB.generated.h"

class ULeaderboard;

class UNakamaClient;
class UNakamaRealtimeClient;
class UNakamaSession;

UCLASS()
class GAMEDATABASE_API AGameDB : public AActor
{
	GENERATED_BODY()

	friend ULeaderboard;
	
public:
	
	AGameDB();
	virtual void BeginPlay() override;
	ULeaderboard& GetLeaderboard() const;

private:

	UFUNCTION()
	void OnAuthenticationSuccess(UNakamaSession* aLoginData);

	UFUNCTION()
	void OnAuthenticationError(const FNakamaError& aError);

	UFUNCTION()
	void OnRealtimeClientConnectSuccess();

	UFUNCTION()
	void OnRealtimeClientConnectError();

	UPROPERTY(EditDefaultsOnly)
	ULeaderboard* myLeaderboard;
	
	UPROPERTY()
	UNakamaClient* myClient;

	UPROPERTY()
	UNakamaRealtimeClient* myRealtimeClient;

	UPROPERTY()
	UNakamaSession* myUserSession;
	
};
