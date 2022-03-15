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

	FVector GetWallNormal() const { return myWallNormal; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunExitDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunMinVelocity = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunJumpMul = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunJumpCoyoteTime = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myOverrideGravityScale = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunGravity = 0.05f;
	
	UPROPERTY(EditDefaultsOnly, Category = "WallClimb")
	float myWallClimbDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "WallClimb")
	float myWallClimbSpeed = 410.0f;

private:
	void UpdateWallNormal() const;
	bool GetIsOverlapping() const;
	
	FVector myWallNormal;
	float myWallrunTimestamp = 0.0f;
	bool myHasHitHead = false;
};
