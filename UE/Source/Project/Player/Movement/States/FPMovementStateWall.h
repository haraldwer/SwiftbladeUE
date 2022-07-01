#pragma once

#include "CoreMinimal.h"
#include "FPMovementStateBase.h"
#include "FPMovementStateWall.generated.h"

UENUM()
enum EFPMovementWallState
{
	SIT,
	CLIMB,
	RUN
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPMovementStateWall : public UFPMovementStateBase
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
	UPROPERTY(EditDefaultsOnly, Category = "WallGeneral")
	float myWallExitDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "WallGeneral")
	float myWallMinVelocity = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "WallGeneral")
	float myOverrideGravityScale = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "WallGeneral")
	float myWallGravity = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "WallJump")
	float myWallJumpMul = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallJump")
	float myWallJumpForwardPart = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "WallJump")
	float myWallJumpCoyoteTime = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myMaxSpeedSlowdown = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "WallClimb")
	float myWallClimbLookDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "WallClimb")
	float myWallClimbSpeed = 410.0f;

	UPROPERTY(EditDefaultsOnly, Category = "WallSit")
	float myWallSitSlowdown = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallSit")
	float myWallSitMoveDot = 0.9f;

	UPROPERTY(EditDefaultsOnly, Category = "WallDetection")
	float mySweepHeight = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallDetection")
	float mySweepRadius = 30.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallDetection")
	float mySweepDepth = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallDetection")
	float myTargetDistance = 10.0f;

private:

	void UpdateMovementNormal() const;
	
	bool GetIsOverlapping() const;
	bool GetHitHead() const;
	bool FindWallInfo(FVector& aNormal, float& aDistance) const;
	static bool IsValidHit(const FHitResult& aHit);
	
	FVector myWallNormal = FVector::ZeroVector;
	bool myHasWallJumped = false;
	float myWallrunTimestamp = 0.0f;
};
