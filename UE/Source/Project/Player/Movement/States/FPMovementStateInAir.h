#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateInAir.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateInAir : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	virtual UClass* Update(float aDT) override;

	virtual UClass* Input(EFPMovementInputAction anAction, float aValue) override;
	virtual UClass* OnLanded() override;
	
	bool HasAirJumps() const;
	void ResetJumps();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "InAir")
	float myCoyoteTime = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "InAir")
	int myNumAirJumps = 1;
	
private:
	float myCoyoteTimeStamp = 0.2f;
	int myAirJumpCount = 1;
	bool myJumpHeld = false;
};
