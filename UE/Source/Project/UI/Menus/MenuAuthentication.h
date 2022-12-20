#pragma once

#include "Data/AuthenticationData.h"
#include "MenuBase.h"
#include "MenuAuthentication.generated.h"

UCLASS(Blueprintable)
class PROJECT_API UMenuAuthentication : public UMenuBase
{
	GENERATED_BODY()

public:

	virtual ~UMenuAuthentication() override = default;
	
	UFUNCTION(BlueprintCallable)
	void Authenticate(const FAuthData& someData);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAuthSuccess(bool aIsCreated); 

	UFUNCTION(BlueprintImplementableEvent)
	void OnAuthFailed(const FString& anError);

};
