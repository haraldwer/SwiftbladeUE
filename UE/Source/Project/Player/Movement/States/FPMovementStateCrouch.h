#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateCrouch.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateCrouch : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	virtual UClass* Update(float aDT) override;
	virtual UClass* Check() override;

	virtual UClass* Input(EFPMovementInputAction anAction, float aValue) override;
	virtual UClass* OnLanded() override;
	
	virtual void Enter() override;
	virtual void Exit() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Crouch")
	float myCamHeightMul = 0.5f;
	
private:
	UClass* CheckCrouch(bool aLanded = false) const;
	
	bool myIsHoldingCrouch = false;
};
