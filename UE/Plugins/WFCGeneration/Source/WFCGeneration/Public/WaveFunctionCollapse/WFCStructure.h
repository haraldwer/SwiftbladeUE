#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCStructure.generated.h"

class AWFCNodeType;
class AWFCNode;

UCLASS()
class WFCGENERATION_API AWFCStructure : public AActor
{
	GENERATED_BODY()

public:

	AWFCStructure();

	UFUNCTION(CallInEditor)
	void Setup();
	
	UFUNCTION(CallInEditor)
	void Solve();
	
	UFUNCTION(CallInEditor)
	void TryIterate();

	UFUNCTION(BlueprintPure)
	AWFCNode* GetNode(int32 anIndex) const;

	UFUNCTION(BlueprintPure)
	virtual TArray<int32> GetNeighbours(int32 anIndex) const { return {}; }
	
protected:

	// Implemented for each structure type
	virtual void CreateStructure() {}
	AWFCNode* CreateNode(const FTransform& aTransform);

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AWFCNodeType>> myTypes;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWFCNode> myNodeType;
	
private:

	void DebugDraw(); 
	bool Iterate();
	void PopulateInitialStack();
	void UpdatePossibilities() const;
	bool EvaluateType(int32 anIndex, const TSubclassOf<AWFCNodeType>& aType) const;
	int32 LeastPossibilities() const;
	
	// All nodes in structure
	TArray<TObjectPtr<AWFCNode>> myNodes;

	// Nodes to solve
	TSet<int32> myStack;

	TArray<TWeakObjectPtr<AWFCNodeType>> mySpawnedActors; 
	
};
