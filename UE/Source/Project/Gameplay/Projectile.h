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
	float myLifetime;

	UPROPERTY(EditDefaultsOnly)
	FVector myVelocity;

private:

	float myStartTime = 0.0f;
	FVector myPreviousLocation = FVector::ZeroVector;
	TWeakObjectPtr<AActor> myInstigator;
	
};
