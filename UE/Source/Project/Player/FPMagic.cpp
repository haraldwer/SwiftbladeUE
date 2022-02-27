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

	for (int i = myPickups.Num() - 1; i >= 0; i--)
	{
		const auto pickup = myPickups[i].Get();
		if (pickup)
			continue;
		myPickups.RemoveAt(i);
	}

	const auto& character = GetCharacter();
	const auto leftHand = character.GetLeftHand();
	const auto localPos = leftHand->GetActorLocation();
	FVector targetDir;
	bool foundTarget = false;
	for (auto& it : myPickups)
	{	
		const auto pickup = it.Get();
		CHECK_CONTINUE_LOG(!pickup, "Pickup null");
		const auto pos = pickup->GetActorLocation();
		const auto diff = pos - localPos;
		const auto dist = diff.Size();
		CHECK_CONTINUE(dist < myRange)
		
		if (dist < myPickupRange)
		{
			// Pickup magic
			myTimestamp = GetWorld()->GetTimeSeconds();
			pickup->Destroy();
			LOG("Pickup was picked up")
			continue;
		}

		foundTarget = true;
		const float weight = 1.0f - (myRange / dist);
		const auto normalized = (diff * (1.0f / dist));
		targetDir += normalized * weight;
		const auto velocity = normalized * weight;
		pickup->SetActorLocation(pickup->GetActorLocation() + velocity);
	}
	
	if (foundTarget)
	{
		// Move hands
	}

	const float currentTime = GetWorld()->GetTimeSeconds();
	myHasMagic = currentTime < myTimestamp + myMagicTime;
	// Shader params are set in BP
}

void UFPMagic::AddPickup(APickup* aPickup)
{
	myPickups.Add(aPickup);
	LOG("Pickup added");
}
