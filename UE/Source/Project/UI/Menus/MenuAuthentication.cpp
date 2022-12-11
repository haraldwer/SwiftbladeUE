#include "MenuAuthentication.h"

#include "Components/Authentication.h"
#include "GameDatabase.h"
#include "Project/Utility/MainSingelton.h"

void UMenuAuthentication::Authenticate(const FAuthData& someData)
{
	auto& auth = UMainSingelton::GetGameDB().GetAuthentication();
	auth.myOnAuthSuccess.AddUniqueDynamic(this, &UMenuAuthentication::OnAuthSuccess);
	auth.myOnAuthError.AddUniqueDynamic(this, &UMenuAuthentication::OnAuthFailed);
	auth.Authenticate(someData);	
}
