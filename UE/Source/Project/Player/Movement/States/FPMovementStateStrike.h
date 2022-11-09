#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateStrike.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateStrike : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	virtual UClass* Update(float aDT) override;
	virtual void Enter() override;
	virtual void Exit() override;

	void SetTarget(const FVector& aTarget) { myTarget = aTarget; }

	virtual bool RequiresMagic() const override { return true; }
	virtual int32 Priority() const override { return 9; }

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Strike")
	float myStrikeVelocity = 1500.0f;

private:
	
	FVector myTarget;
	
};
