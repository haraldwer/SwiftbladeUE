#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateWallrun.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateWallrun : public UFPMovementStateBase
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	virtual UClass* Update(float aDT) override;
	
	virtual void Enter() override;
	virtual void Exit() override;

	virtual UClass* OnLanded() override;
	virtual UClass* OnHit(const FHitResult& aHit) override;
	
	bool GetCanWallJump() const;
	FVector GetWalljumpDirection() const;
	void OnWallJump();

	FVector GetWallNormal() const { return myWallNormal; }

	virtual bool RequiresMagic() const override { return true; }
	virtual TSubclassOf<UFPAnimationStateBase> GetAnimation() const override;
	virtual int32 Priority() const override { return 4; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunExitDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunMinVelocity = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunJumpMul = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunJumpForwardPart = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunJumpCoyoteTime = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myOverrideGravityScale = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunGravity = 5.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myMaxSpeedSlowdown = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "WallClimb")
	float myWallClimbDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "WallClimb")
	float myWallClimbSpeed = 410.0f;

private:
	void UpdateWallNormal() const;
	bool GetIsOverlapping() const;
	
	FVector myWallNormal;
	bool myHasWallJumped = false;
	float myWallrunTimestamp = 0.0f;
	bool myHasHitHead = false;
};
