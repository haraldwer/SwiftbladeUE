#include "MenuAuthentication.h"

#include "Components/Authentication.h"
#include "GameDatabase.h"

void UMenuAuthentication::Authenticate(const FAuthData& someData)
{
	auto& auth = UGameDatabase::Get().GetAuthentication();
	auth.myOnAuthSuccess.AddUniqueDynamic(this, &UMenuAuthentication::OnAuthSuccess);
	auth.myOnAuthError.AddUniqueDynamic(this, &UMenuAuthentication::OnAuthFailed);
	auth.Authenticate(someData);	
}
