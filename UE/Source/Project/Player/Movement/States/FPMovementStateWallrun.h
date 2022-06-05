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

	virtual bool RequiresMagic() const override { return true; }
	virtual int32 Priority() const override { return 4; }
	
	bool GetCanWallJump() const;
	FVector GetWalljumpDirection() const;
	void OnWallJump();

	FVector GetWallNormal() const { return myWallNormal; }

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

	UPROPERTY(EditDefaultsOnly, Category = "WallDetection")
	float myTestWidth = 200.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallDetection")
	float myTestHeight = 200.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallDetection")
	float myTargetDistance = 45.0f;

private:
	void UpdateMovementNormal() const;
	
	bool GetIsOverlapping() const;
	bool GetHitHead() const;
	bool FindWallInfo(FVector& aNormal, float& aDistance) const;
	static bool IsValidHit(const FHitResult& aHit);
	
	FVector myWallNormal;
	bool myHasWallJumped = false;
	float myWallrunTimestamp = 0.0f;
};
