#pragma once

#include "CoreMinimal.h"
#include "FPComponentBase.h"
#include "FPMagic.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UFPMagic : public UFPComponentBase
{
	GENERATED_BODY()

public:	
	UFPMagic();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddPickup(class APickup* aPickup);

	UFUNCTION(BlueprintPure)
	bool HasMagic() const { return myHasMagic; }

protected:
	UPROPERTY(EditDefaultsOnly)
	float myRange = 300.0f;

	UPROPERTY(EditDefaultsOnly)
	float myPickupRange = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float mySpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float myMagicTime = 3.0f;

private:

	// All pickups in world are cached here
	UPROPERTY()
	TArray<TWeakObjectPtr<class APickup>> myPickups;

	float myTimestamp = 0.0f;
	bool myHasMagic = false;
};
