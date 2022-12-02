#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProcSlice.generated.h"

USTRUCT(BlueprintType)
struct FSliceEdge
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FVector myNormal = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite)
	FVector myLocation = FVector::ZeroVector;
};

UINTERFACE()
class UProcSlice : public UInterface
{
	GENERATED_BODY()
};

class PROJECT_API IProcSlice
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Slice(const TArray<FSliceEdge>& someEdges);
	
};
