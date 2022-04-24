#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateSlide.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateSlide : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	virtual UClass* Update(float aDT) override;
	virtual void Enter() override;
	virtual void Exit() override;

	virtual bool RequiresMagic() const override { return true; }
	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual int32 Priority() const override { return 3; }

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float mySlideDuration = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float mySlideMinDot = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float mySlideSpeedMul = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crouch")
	float myCamHeightMul = 0.5f;
	
private:
	float mySlideTimeStamp = 0.0f;
	FVector mySlideDirection;
};
