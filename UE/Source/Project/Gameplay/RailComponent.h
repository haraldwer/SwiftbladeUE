#pragma once

#include "CoreMinimal.h"
#include "RailComponent.generated.h"

class USplineComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API URailComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	URailComponent();

	virtual void BeginPlay() override;

	USplineComponent* GetSpline() const { return mySpline.Get(); };

private:

	TWeakObjectPtr<USplineComponent> mySpline;
	
};
