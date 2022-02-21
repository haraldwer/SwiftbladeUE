#include "FPMagic.h"

UFPMagic::UFPMagic()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFPMagic::BeginPlay()
{
	Super::BeginPlay();
}

void UFPMagic::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	const float rangeSqr = myRange * myRange;
	const auto localPos = GetComponentLocation();
	for (auto& it : myPickups)
	{	
		const auto pickup = it.Get();
		CHECK_CONTINUE(!pickup);
		const auto pos = it->GetActorLocation();
		const auto diff = pos - localPos;
		const auto dist = diff.SizeSquared();
		CHECK_CONTINUE(dist > rangeSqr);
		LOG("Pickup");
	}
}

void UFPMagic::AddPickup(APickup* aPickup)
{
	myPickups.Add(aPickup);
}
