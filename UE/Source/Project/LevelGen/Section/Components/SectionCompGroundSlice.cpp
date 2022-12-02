#include "SectionCompGroundSlice.h"

#include "Project/LevelGen/Section/SectionGenerator.h"
#include "Project/ProcGen/ProcSlice.h"

void USectionCompGroundSlice::PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom)
{
	const FVector actorLoc = FVector(aRoom.location.X, aRoom.location.Y, aRoom.groundOffset); 
	AActor* actor = aGenerator->SpawnGeneratedActor(mySliceClass, FTransform(actorLoc));
	CHECK_RETURN_LOG(!actor, "No actor spawned");
	CHECK_RETURN_LOG(!actor->Implements<UProcSlice>(), "Does not implement interface");

	actor->SetActorScale3D(FVector(aRoom.radius));

	TArray<FSliceEdge> sliceEdges;
	for (const auto& edge : aRoom.edges)
	{
		auto& entry = sliceEdges.Emplace_GetRef();
		entry.myLocation = FVector(edge.location.X, edge.location.Y, aRoom.groundOffset);
		entry.myNormal = FVector(-edge.normal.X, -edge.normal.Y, mySliceTilt);
	}

	if (mySliceHeight)
	{
		auto& entry = sliceEdges.Emplace_GetRef();
		entry.myLocation = actorLoc;
		entry.myNormal = FVector::DownVector;
	}
	
	IProcSlice::Execute_Slice(actor, sliceEdges);

	Super::PopulateRoom(aGenerator, aSection, aRoom);
}
