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
	
	void Jump();
	void Dash();
	
	void Grapple();
	void Landed(const FHitResult& aHit);
	void StartWallrun(FVector aNormal);
	void StopWallrun();
	void Wallrun(float aDT);

	FVector GetWallNormal() const;
	
private:
	class AFPCharacter* GetFPCharacter() const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Wallrunning")
	class UCapsuleComponent* myWallDetection;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myWallrunExitDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float myWallrunMinVelocity = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myWallrunJumpMul = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myWallClimbDot = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myWallClimbTime = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myWallClimbSpeed = 410.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myCoyoteTime = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	int myNumAirJumps = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myDashImpuleStrength = 1024.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myDashVerticalMul = 0.25f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myDashDuration = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myDashFov = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myGrappleDist = 2048.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myGrappleDot = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myGrappleForceMul = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myGrappleVerticalMul = 0.75f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float myGrappleCooldown = 1.0f;
	
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

	// General
	float myCoyoteTimer = 0.0f;
	int myAirJumpCount = 0;
	bool myHasDashed = false;
	float myDashTimer = 0.0f;
	float myGrappleTimer = 0.0f;
	
};
