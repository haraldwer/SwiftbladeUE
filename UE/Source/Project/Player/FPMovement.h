#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPMovement.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UFPMovement : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFPMovement();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Input
	void PressJump();
	void ReleaseJump();
	
	void PressCrouch();
	void ReleaseCrouch();
	
	void Jump();
	void Dash();	
	void Grapple();
	
	void Landed(const FHitResult& aHit);
	
	void StartWallrun(FVector aNormal);
	void StopWallrun();
	void Wallrun(float aDT);

	void UpdateCrouch(float aDT);
	void UpdateDash(float aDT);

	void StartSlide(FVector aVelocity);
	void StopSlide();
	
	FVector GetWallNormal() const;
	
private:
	class AFPCharacter* GetFPCharacter() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="WallRun")
	class UCapsuleComponent* myWallDetection;
	
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunExitDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category="WallRun")
	float myWallrunMinVelocity = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	float myWallrunJumpMul = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "WallClimb")
	float myWallClimbDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "WallClimb")
	float myWallClimbTime = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "WallClimb")
	float myWallClimbSpeed = 410.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myCoyoteTime = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	int myNumAirJumps = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myCameraHeightSmoothing = 15.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float myDashVelocity = 1024.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float myDashVerticalMul = 0.25f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float myDashDuration = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float myDashFov = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myGrappleDist = 2048.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myGrappleDot = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myGrappleForceMul = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myGrappleVerticalMul = 0.75f;
	UPROPERTY(EditDefaultsOnly, Category = "Grapple")
	float myGrappleCooldown = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Crouch")
	float myCrouchCamHeightMul = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float mySlideDuration = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float mySlideMinDot = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float mySlideSpeedMul = 1.5f;

private:
	// B-Hopping
	bool myHoldingJump = false;

	// Wall running
	bool myIsWallRunning = false;
	bool myWasWallRunning = false;
	FVector myWallNormal;
	bool myHasHitHead = false;
	
	// Wall climbing
	float myPrevWallZVel = 0.0f;

	// Dash
	FVector myDashDirection;
	bool myHasDashed = false;
	float myDashTimer = 0.0f;

	// Grapple
	float myGrappleTimer = 0.0f;

	// Crouch
	float mySlideTimer = 0.0f;
	bool myIsSliding = false;
	FVector mySlideDirection;
	
	// General
	float myCoyoteTimer = 0.0f;
	int myAirJumpCount = 0;
	float myCameraHeight = 0.0f;
	float myOriginalCameraHeight = 0.0f;
};
