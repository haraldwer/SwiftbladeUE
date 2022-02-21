#include "Pickup.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Player/FPMagic.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;	
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	auto localPlayer = UMainSingelton::GetLocalPlayer();
	CHECK_RETURN_LOG(localPlayer, "No local player");
	auto magic = localPlayer->GetMagic();
	CHECK_RETURN_LOG(magic, "No magic component");
	magic->AddPickup(this);
}
