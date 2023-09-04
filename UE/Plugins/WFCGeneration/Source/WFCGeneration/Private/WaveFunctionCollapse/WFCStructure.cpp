
#include "WaveFunctionCollapse/WFCStructure.h"

#include "WaveFunctionCollapse/WFCNode.h"
#include "Utility/Utility.h"
#include "WaveFunctionCollapse/WFCNodeType.h"
#include "WaveFunctionCollapse/WFCRule.h"

AWFCStructure::AWFCStructure()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

void AWFCStructure::DebugDraw()
{
	FlushPersistentDebugLines(GetWorld()); 
	for (auto node : myNodes)
		if (const auto ptr = node.Get())
			ptr->DebugDraw();
}

void AWFCStructure::Setup()
{
	for (const auto node : myNodes)
		if (const auto ptr = node.Get())
			ptr->Destroy();
	for (const auto spawned : mySpawnedActors)
		if (const auto ptr = spawned.Get())
			ptr->Destroy();
	myStack.Reset(); 
	myNodes.Reset(); 
		
	CreateStructure();
	PopulateInitialStack();
	DebugDraw(); 
}

void AWFCStructure::PopulateInitialStack()
{
	CHECK_RETURN_LOG(!myNodes.Num(), "No nodes when populating stack");
	
	for (int32 i = 0; i < myNodes.Num(); i++)
	{
		const auto ptr = myNodes[i].Get();
		CHECK_CONTINUE(!ptr);
		CHECK_CONTINUE(!IsValid(ptr->mySolution));
		for (int32 neighbor : GetNeighbours(i))
			myStack.Add(neighbor);
	}

	if (!myStack.Num())
		myStack.Add(FMath::RandRange(0, myNodes.Num() - 1));
	UpdatePossibilities();
}

void AWFCStructure::Solve()
{
	constexpr int32 maxIterations = 10000;
	for (int32 i = 0; i < maxIterations; i++)
		if (!Iterate())
			break;
	DebugDraw(); 
}

void AWFCStructure::TryIterate()
{
	Iterate();
	DebugDraw();
}

bool AWFCStructure::Iterate()
{
	if (!myStack.Num())
		return false;
	
	// Select the one with least possibilities
	const auto index = LeastPossibilities();
	CHECK_RETURN_LOG(index < 0, "Failed to find node with least possibilities", false); 
	const auto node = myNodes[index].Get();
	myStack.Remove(index);
	
	// Select solution
	const int32 numPossibilities = node->myPossibilities.Num();
	CHECK_RETURN_LOG(numPossibilities <= 0, "No possible solutions", false);
	const int32 solution = numPossibilities == 1 ?
		0 : FMath::RandRange(0, numPossibilities - 1); // TODO: Weighted randomness
	
	CHECK_RETURN_LOG(!node->myPossibilities.IsValidIndex(solution), "Invalid solution index", false);
	node->mySolution = node->myPossibilities[solution]; 
	node->myPossibilities.Reset();

	// Apply 
	if (const auto spawnedNode = node->ApplySolution())
		mySpawnedActors.Add(spawnedNode); 

	// Add neighbors to stack
	for (const int32 neighborIndex : GetNeighbours(index))
	{
		if (const auto neighbor = GetNode(neighborIndex))
			if (IsValid(neighbor->mySolution.Get()))
				continue;
		myStack.Add(neighborIndex);
	}

	// Purge possibilities 
	UpdatePossibilities();

	return true; 
}

AWFCNode* AWFCStructure::GetNode(const int32 anIndex) const
{
	if (myNodes.IsValidIndex(anIndex))
		return myNodes[anIndex].Get();
	return nullptr; 
}

void AWFCStructure::UpdatePossibilities() const
{
	for (const int32 index : myStack)
	{
		CHECK_CONTINUE_LOG(!myNodes.IsValidIndex(index), "Invalid index");
		const auto node = myNodes[index].Get();
		CHECK_CONTINUE_LOG(!node, "Invalid node");

		// Get current possibilities
		TArray<TSubclassOf<AWFCNodeType>> possibilities =
			node->myPossibilities.Num() > 0 ?
				node->myPossibilities : myTypes;

		// Apply rules
		TArray<TSubclassOf<AWFCNodeType>> result;  
		for (const TSubclassOf<AWFCNodeType>& type : possibilities)
			if (EvaluateType(index, type))
				result.Add(type);

		node->myPossibilities = result; 
	}
}

bool AWFCStructure::EvaluateType(const int32 anIndex, const TSubclassOf<AWFCNodeType>& aType) const
{
	// Get the rules for each type
	const auto typePtr = aType.GetDefaultObject();
	CHECK_RETURN_LOG(!IsValid(typePtr), "Invalid type", false)
	for (const auto rule : typePtr->myRules)
	{
		const auto rulePtr = rule.GetDefaultObject();
		CHECK_CONTINUE_LOG(!IsValid(rulePtr), "Invalid rule")
		if (!rulePtr->Evaluate(this, anIndex))
			return false; 
	}
	return true;
}

int32 AWFCStructure::LeastPossibilities() const
{
	int32 minPossibilities = -1;
	int32 minIndex = -1; 
	for (const int32 index : myStack)
	{
		CHECK_CONTINUE_LOG(!myNodes.IsValidIndex(index), "Invalid index"); 
		const auto node = myNodes[index].Get();
		CHECK_CONTINUE_LOG(!node, "Invalid node");
		const int32 possibilities = node->myPossibilities.Num(); 
		if (minIndex < 0 || minPossibilities < 0 ||
			possibilities < minPossibilities)
		{
			minPossibilities = possibilities;
			minIndex = index; 
		}
	}
	return minIndex; 
}

AWFCNode* AWFCStructure::CreateNode(const FTransform& aTransform)
{
	auto node = Cast<AWFCNode>(GetWorld()->SpawnActor(myNodeType, &aTransform));
	myNodes.Add(node);
	return node; 
}
