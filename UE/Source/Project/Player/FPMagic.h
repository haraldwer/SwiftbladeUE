#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FPMagic.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UFPMagic : public USceneComponent
{
	GENERATED_BODY()

public:	
	UFPMagic();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddPickup(class APickup* aPickup);

	bool HasMagic() const { return true; }

protected:
	UPROPERTY(EditDefaultsOnly)
	float myRange = 300.0f;

	UPROPERTY(EditDefaultsOnly)
	float mySpeed = 10.0f;

private:

	// All pickups in world are cached here
	TArray<TWeakObjectPtr<class APickup>> myPickups;
};
