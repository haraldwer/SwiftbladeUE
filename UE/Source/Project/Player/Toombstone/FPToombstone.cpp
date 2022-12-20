#include "FPToombstone.h"

#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"
#include "GameDatabase.h"
#include "Components/Blob.h"
#include "Project/Utility/MainSingelton.h"

UFPToombstone::UFPToombstone()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f; 
}

void UFPToombstone::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult hit;
	const FVector loc = GetCharacter().GetActorLocation();
	if (GetWorld()->LineTraceSingleByChannel(hit, loc, loc + FVector::DownVector * myTraceDist, ECC_Visibility))
		myLastValidLocation = hit.Location;
}

void UFPToombstone::CreateStones() const
{
	const auto& blob = UMainSingelton::GetGameDB().GetBlob();
	auto data = blob.Get();
	const int32 seed = GetController().GetState().mySeed;
	for (auto& stone : data.myToombstoneData.myToombstones)
	{
		if (stone.mySeed != seed)
			continue;
		const auto rot = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
		GetWorld()->SpawnActor(myToombstoneClass, &stone.myLocation, &rot);
	}
}

void UFPToombstone::StoreLocation() const
{
	auto& blob = UMainSingelton::GetGameDB().GetBlob();
	auto data = blob.Get();
	auto& stones = data.myToombstoneData.myToombstones;
	
	FToombstoneEntry entry;
	entry.myLocation = myLastValidLocation;
	entry.mySeed = GetController().GetState().mySeed; 
	
	stones.Insert(entry, 0);
	while (stones.Num() > myNumToombstones)
		stones.RemoveAt(0);
	blob.Set(data);
}

