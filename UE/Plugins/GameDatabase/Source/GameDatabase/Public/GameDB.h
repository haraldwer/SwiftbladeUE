#pragma once

#include "CoreMinimal.h"
#include "NakamaClient.h"

#include "GameDB.generated.h"

UCLASS()
class GAMEDATABASE_API AGameDB : public AActor
{
	GENERATED_BODY()

public:
	AGameDB();
	virtual void BeginPlay() override;

private:
	
	UPROPERTY()
	UNakamaClient* NakamaClient;

	UPROPERTY()
	UNakamaRealtimeClient* NakamaRealtimeClient;

	UPROPERTY()
	UNakamaSession* UserSession;

	UFUNCTION()
	void OnAuthenticationSuccess(UNakamaSession* aLoginData);

	UFUNCTION()
	void OnAuthenticationError(const FNakamaError& aError);

	UFUNCTION()
	void OnRealtimeClientConnectSuccess();

	UFUNCTION()
	void OnRealtimeClientConnectError();
	
};
