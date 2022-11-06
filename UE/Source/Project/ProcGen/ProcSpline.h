#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProcSpline.generated.h"

class USplineComponent;

UINTERFACE(MinimalAPI, Blueprintable)
class UProcSplineMesh : public UInterface
{
	GENERATED_BODY()
};

class PROJECT_API IProcSplineMesh
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	USplineComponent* GetSpline(int32 anIndex) const;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void GenerateMesh();
	
};
