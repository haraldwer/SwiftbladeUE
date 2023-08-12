#include "FPToombstone.h"

#include "Project/Player/FPCharacter.h"
#include "GameDatabase.h"
#include "Components/Blob.h"
#include "Project/Player/FPStateSubsystem.h"

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
	const auto& blob = UGameDatabase::Get().GetBlob();
	const auto data = blob.Get();
	const auto state = UFPStateSubsystem::Get().GetState();
	for (const auto& stone : data.myToombstoneData.myToombstones)
	{
		if (stone.mySeed != state.mySeed ||
			stone.myArenaIndex != state.myArenaIndex ||
			stone.myInArena != state.myInArena)
			continue;
		
		const auto rot = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
		GetWorld()->SpawnActor(myToombstoneClass, &stone.myLocation, &rot);
	}
}

void UFPToombstone::StoreLocation() const
{
	auto& blob = UGameDatabase::Get().GetBlob();
	auto data = blob.Get();
	auto& stones = data.myToombstoneData.myToombstones;
	const auto state = UFPStateSubsystem::Get().GetState();
	
	FToombstoneEntry entry;
	entry.myLocation = myLastValidLocation;
	entry.mySeed = state.mySeed;
	entry.myArenaIndex = state.myArenaIndex;
	entry.myInArena = state.myInArena;
	
	stones.Add(entry);
	while (stones.Num() > myNumToombstones)
		stones.RemoveAt(0);
	blob.Set(data);
}
