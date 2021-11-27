#include "Effect.h"
#include "NiagaraComponent.h"

AEffect::AEffect()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEffect::BeginPlay()
{
	Super::BeginPlay();
	StartParticleCounter();
}

void AEffect::StartParticleCounter()
{
	myTotalParticles = 0;
	myParticlesFinished = 0;
	TArray<UActorComponent*> comps = GetComponentsByClass(UNiagaraComponent::StaticClass());
	for (const auto comp : comps)
	{
		const auto ps = Cast<UNiagaraComponent>(comp);
		CHECK_CONTINUE(!ps);
		ps->OnSystemFinished.RemoveAll(this);
		ps->OnSystemFinished.AddDynamic(this, &AEffect::SystemFinished);
		myTotalParticles++;
	}
}

void AEffect::SystemFinished(UNiagaraComponent* aComponent)
{
	if (aComponent)
		aComponent->OnSystemFinished.RemoveAll(this);
	myParticlesFinished++;
	if (myParticlesFinished >= myTotalParticles)
		Destroy();
}

