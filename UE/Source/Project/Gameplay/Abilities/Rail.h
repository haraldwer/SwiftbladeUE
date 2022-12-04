#pragma once

#include "CoreMinimal.h"
#include "Rail.generated.h"

class USplineComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API URail : public UActorComponent
{
	GENERATED_BODY()

public:
	
	URail();

	virtual void BeginPlay() override;

	USplineComponent* GetSpline() const { return mySpline.Get(); };

private:

	TWeakObjectPtr<USplineComponent> mySpline;
	
};
