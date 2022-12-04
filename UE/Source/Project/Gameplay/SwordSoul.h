#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwordSoul.generated.h"

UCLASS()
class PROJECT_API ASwordSoul : public AActor
{
	GENERATED_BODY()

public:
	
	ASwordSoul();
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float myDistanceOffset = 500.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float mySmoothing = 10.0f;


	
	
};
