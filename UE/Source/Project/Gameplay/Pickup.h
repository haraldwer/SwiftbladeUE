#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class PROJECT_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void BeginPlay() override;
};
