#pragma once

#include "Data/AuthenticationData.h"
#include "DBComponent.h"
#include "NakamaClient.h"

#include "Authentication.generated.h"

UCLASS(ClassGroup=(GameDB))
class GAMEDATABASE_API UAuthentication : public UDBComponent
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuthSuccess, bool, aIsCreated);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuthError, const FString&, anError);
	
	void Authenticate();
	void Authenticate(const FAuthData& someData);
	FOnAuthSuccess myOnAuthSuccess;
	FOnAuthError myOnAuthError;

	// TODO: Handle linking

	bool IsAuthenticated() const { return GetSession() != nullptr; }

private:

	UFUNCTION()
	void OnAuthSuccess(UNakamaSession* aSession);

	UFUNCTION()
	void OnAuthError(const FNakamaError& aError);

	static bool LoadAuthData(FAuthData& someData);
	static bool SaveAuthData(const FAuthData& someData);

	FAuthData myAuthData;

	static constexpr TCHAR* myFileName = TEXT("auth");
	
};
