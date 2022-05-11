#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class PROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	
	AProjectile();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetInstigator(AActor* anInstigator) { myInstigator = anInstigator; }

	UFUNCTION(BlueprintImplementableEvent)
	void CreateHitEffects();
	
protected:

	UPROPERTY(EditDefaultsOnly)
	float myLifetime = 1.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myInitialDelay = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	FVector myVelocity = FVector::ZeroVector;

private:

	float myStartTime = 0.0f;
	FVector myPreviousLocation = FVector::ZeroVector;
	TWeakObjectPtr<AActor> myInstigator;
	
};
