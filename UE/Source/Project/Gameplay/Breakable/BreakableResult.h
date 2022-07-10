#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableResult.generated.h"

UCLASS()
class PROJECT_API ABreakableResult : public AActor
{
	GENERATED_BODY()

public:
	ABreakableResult();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BreakMeshes(const TArray<UStaticMeshComponent*>& someComponents, const FVector& aPosition, const FVector& aNormal, bool aSplit);

};
