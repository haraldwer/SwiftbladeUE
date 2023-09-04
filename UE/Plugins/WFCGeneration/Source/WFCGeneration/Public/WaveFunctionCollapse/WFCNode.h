#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCNode.generated.h"

class AWFCNodeType;

UCLASS()
class WFCGENERATION_API AWFCNode : public AActor
{
	GENERATED_BODY()

	friend class AWFCStructure;
	
public:

	AWFCNode();
	
	void DebugDraw() const;
	AWFCNodeType* ApplySolution() const;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AWFCNodeType> mySolution;

private:

	TArray<TSubclassOf<AWFCNodeType>> myPossibilities;
	
};
