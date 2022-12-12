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
	float myDistanceOffset = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	float myOffsetNoiseScale = 0.01f;
	
	UPROPERTY(EditDefaultsOnly)
	float myOffsetNoiseTimeScale = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	float myOffsetNoiseStrength = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	FVector myOffsetMul = FVector::OneVector; 

	UPROPERTY(EditDefaultsOnly)
	float mySmoothing = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	float myRotSmoothing = 10.0f;

private:
	
	static FTransform GetSplineTrans(FVector aLocation, float aDistanceOffset = 0.0f);

	float myDistance = 0.0f; 
	
};
