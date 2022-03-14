#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Effect.generated.h"

UCLASS()
class PROJECT_API AEffect : public AActor
{
	GENERATED_BODY()

public:
	AEffect();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	bool myDestroyOnAllParticlesFinished = true;

private:
	void StartParticleCounter();

	UFUNCTION()
	void SystemFinished(class UNiagaraComponent* aComponent);
	
	int myTotalParticles = 0;
	int myParticlesFinished = 0;
};
