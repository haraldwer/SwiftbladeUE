
#include "WaveFunctionCollapse/WFCNode.h"

#include "Utility/Utility.h"
#include "WaveFunctionCollapse/WFCNodeType.h"

AWFCNode::AWFCNode()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

void AWFCNode::DebugDraw() const
{
	const auto world = GetWorld();
	CHECK_RETURN(!world); 

	FColor color = FColor::White;
	if (myPossibilities.Num() == 1)
		color = FColor::Blue;
	if (myPossibilities.Num() == 2)
		color = FColor::Green;
	if (myPossibilities.Num() > 2)
		color = FColor::Red;
	
	DrawDebugBox(
		world,
		GetActorLocation(),
		FVector::One() * 49.0f,
		GetActorRotation().Quaternion(),
		color,
		true);
}

AWFCNodeType* AWFCNode::ApplySolution() const
{
	const auto solution = mySolution.Get();
	CHECK_RETURN_LOG(!IsValid(solution), "Invalid solution", nullptr);
	const auto world = GetWorld();
	CHECK_RETURN(!world, nullptr);
	const FTransform trans = GetTransform(); 
	return Cast<AWFCNodeType>(world->SpawnActor(solution, &trans));
}

