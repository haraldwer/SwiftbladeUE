#pragma once

#include "CoreMinimal.h"
#include "Project/Player/FPComponentBase.h"
#include "FPAnimator.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UFPAnimator : public UFPComponentBase
{
	GENERATED_BODY()

public:
	
	UFPAnimator();

protected:
	
	virtual void BeginPlay() override;
	
public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	enum class State
	{
		IDLE,
		RUNNING,
		WALL_RUNNING,
		WALL_CLIMBING,
		FALLING
	};

	void SetState(State aState);
	
	void StartDash();
	void StartJump();
	void StartGrapple();

	FTransform GetRight() const;
	FTransform GetLeft() const;
	float GetSwordPart() const;

private:
	
	void Idle(float aDT);
	void Running(float aDT);
	void WallRunning(float aDT);
	void WallClimbing(float aDT);
	void Falling(float aDT);

	void Jump(float aDT);
	void Dash(float aDT);
	void Grapple(float aDT);
	
public:
	
	UPROPERTY(EditAnywhere, Category = "Hands")
	FVector myDefaultHandLocation;
	UPROPERTY(EditAnywhere, Category = "Hands")
	FVector myDefaultHandRotation;
	UPROPERTY(EditAnywhere, Category = "Hands")
	float myHandSmoothing = 10.0f;

	UPROPERTY(EditAnywhere, Category = "General")
	float myRotationStrength = 0.02f;
	UPROPERTY(EditAnywhere, Category = "General")
	float myPositionStrength = 5.0f;
	UPROPERTY(EditAnywhere, Category = "General")
	float mySpeed = 10.0f;
	UPROPERTY(EditAnywhere, Category = "General")
	float mySmoothing = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Wallrun")
	float myWallrunTilt = 0.175f;

	UPROPERTY(EditAnywhere, Category = "Idle")
	float myIdleScale = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Idle")
	float myIdleSpeed = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Footsteps")
	TSubclassOf<AActor> myFootstepBP;
	
private:
	
	void Step() const;
	
	State myState = State::IDLE;

	// Headbob
	float myTimer = 9999.0f;

	float myHeadRotation = 0.0f;
	float myHeadPosition = 0.0f;
	float myRealHeadRotation = 0.0f;
	float myRealHeadPosition = 0.0f;

	FTransform myRight;
	FTransform myLeft;
	FTransform myRealRight;
	FTransform myRealLeft;

	float myDashTime = 9999.0;
	float myGrappleTime = 9999.0f;
	float myJumpTime = 9999.0f;

	float mySwordPart = 0.0f;

	bool myStepRight = false;
};
