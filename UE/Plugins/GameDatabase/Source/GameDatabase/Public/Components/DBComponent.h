#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DBComponent.generated.h"

UCLASS(ClassGroup=(GameDB), Abstract)
class UDBComponent : public UObject
{
	GENERATED_BODY()

protected:

	class UGameDatabase& GetDB() const;
	class UNakamaClient* GetClient() const;
	class UNakamaRealtimeClient* GetRealtimeClient() const;
	class UNakamaSession* GetSession() const;
	
};
