#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class PROJECT_API ACheckpoint : public AActor
{
	GENERATED_BODY()
public:	
	ACheckpoint();

	UFUNCTION(BlueprintImplementableEvent)
	void Activate(class AFPController* anInstigator);

	UFUNCTION(BlueprintCallable)
	void FinishActivate(class AFPController* anInstigator);
};
