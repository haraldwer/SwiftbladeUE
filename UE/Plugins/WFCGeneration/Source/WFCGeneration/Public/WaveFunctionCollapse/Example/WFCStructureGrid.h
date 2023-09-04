#pragma once

#include "CoreMinimal.h"
#include "WaveFunctionCollapse/WFCStructure.h"
#include "WFCStructureGrid.generated.h"

UCLASS()
class WFCGENERATION_API AWFCStructureGrid : public AWFCStructure
{
	GENERATED_BODY()

protected:

	virtual void CreateStructure() override;
	virtual TArray<int32> GetNeighbours(int32 anIndex) const override;

	UPROPERTY(EditAnywhere)
	int32 myGridSize = 10;

	UPROPERTY(EditAnywhere)
	float myScale = 100.0f; 
	
private:

	int32 GetIndex(int32 anX, int32 aY) const; 
	int32 GetX(int32 anIndex) const;
	int32 GetY(int32 anIndex) const;
	
	
};
