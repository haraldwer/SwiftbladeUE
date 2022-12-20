#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PooledActorInterface.generated.h"

UINTERFACE()
class UPooledActorInterface : public UInterface
{
	GENERATED_BODY()
};

class UTILITY_API IPooledActorInterface
{
	GENERATED_BODY()

public:

	virtual void OnPoolActivate() {}
	virtual void OnPoolDeactivate() {}
};
