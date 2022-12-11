#include "Components/DBComponent.h"
#include "GameDatabase.h"

UGameDatabase& UDBComponent::GetDB() const
{
	const auto db = Cast<UGameDatabase>(GetOuter());
	check(db && "AGameDB null in UDBComponent");
	return *db; 
}

UNakamaClient* UDBComponent::GetClient() const
{
	return GetDB().myClient;
}

UNakamaRealtimeClient* UDBComponent::GetRealtimeClient() const
{
	return GetDB().myRealtimeClient;
}

UNakamaSession* UDBComponent::GetSession() const
{
	return GetDB().myUserSession;
}
