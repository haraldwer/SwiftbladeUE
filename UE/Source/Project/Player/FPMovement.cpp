#include "FPMovement.h"
#include "FPCharacter.h"
#include "Actors/Sword.h"
#include "Animation/FPAnimator.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Utility.h"
#include "Project/Gameplay/GrapplePoint.h"
#include "Project/Gameplay/StickySurface.h"

UFPMovement::UFPMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFPMovement::BeginPlay()
{
	Super::BeginPlay();
	
	myCameraHeight = GetCamera().GetRelativeLocation().Z + GetCapsule().GetScaledCapsuleHalfHeight();
	myOriginalCameraHeight = myCameraHeight;
	
	auto& movement = GetCharacterMovement();
	movement.GravityScale = myOverrideGravityScale;
}

void UFPMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto& movement = GetCharacterMovement();
	auto& animator = GetAnimator(); 

	myCoyoteTimer = myIsWallRunning || movement.IsWalking() ? 
		0.0f : myCoyoteTimer + DeltaTime;

	UpdateCrouch(DeltaTime);
	Wallrun(DeltaTime);
	UpdateDash(DeltaTime);
	UpdateGrapple(DeltaTime);

	if (!myIsWallRunning && !myIsSliding)
	{
		if (movement.IsWalking())
			animator.SetState(movement.GetLastInputVector().Size() > 0.3f ? UFPAnimator::State::RUNNING : UFPAnimator::State::IDLE);
		else animator.SetState(UFPAnimator::State::FALLING);
	}

	// Update camera height
	{
		const float targetCamHeight = myOriginalCameraHeight * 
			(movement.IsCrouching() ? myCrouchCamHeightMul : 1.0f);
		
		myCameraHeight = FMath::FInterpTo(
			myCameraHeight,
			targetCamHeight,
			DeltaTime,
			myCameraHeightSmoothing);

		const float height = myCameraHeight - GetCapsule().GetScaledCapsuleHalfHeight();
		// Feet location + height
		GetCamera().SetRelativeLocation(FVector(0, 0, height));
	}
}

void UFPMovement::PressJump()
{
	LOG("Jump pressed");
	if (myIsWallRunning)
		StopWallrun();
	myHoldingJump = true;
	Jump();
}

void UFPMovement::ReleaseJump()
{
	myHoldingJump = false;
}

void UFPMovement::PressCrouch()
{
	myHoldingCrouch = true;
	StartCrouch();
}

void UFPMovement::ReleaseCrouch()
{
	myHoldingCrouch = false;
	StopCrouch();
}

void UFPMovement::Jump()
{
	auto& movement = GetCharacterMovement();
	const bool onGround = myIsWallRunning || movement.IsWalking() || myCoyoteTimer < myCoyoteTime;
	if(onGround || myAirJumpCount < myNumAirJumps)
	{
		LOG("Jump");
		FVector direction = FVector(0, 0, 1);
		if (myWasWallRunning) direction += myWallNormal * myWallrunJumpMul * 
			(1 - FMath::Abs(FVector::DotProduct(myWallNormal, GetCamera().GetForwardVector())));
		movement.Velocity.Z = 0;
		movement.AddImpulse(direction * movement.JumpZVelocity, true);
		if(!onGround)
			myAirJumpCount++;
		GetAnimator().StartJump();
		StopSlide();
		CreateEffect(myJumpEffect, GetActorTransform());
	}
}

void UFPMovement::Dash()
{
	CHECK_RETURN(myHasDashed);

	StopSlide();
	
	auto& movement = GetCharacterMovement();
	auto vel = GetCamera().GetForwardVector();
	if (movement.IsWalking() && vel.Z < 0.1f)
		vel.Z = 0.1f;
	const auto input = GetCharacter().GetLastMovementInputVector();
	if(input.Size() > 0.3f)
		vel = FVector(input.X, input.Y, vel.Z);
	vel.Normalize();
	vel.Z += 0.1f * (1 - abs(vel.Z));
	vel.Z *= myDashVerticalMul;

	myDashDirection = vel;
	movement.Velocity = vel * myDashVelocity;
	myHasDashed = true;
	myDashTimer = 0;
	
	GetAnimator().StartDash();
}


void UFPMovement::Grapple()
{
	CHECK_RETURN(myGrappleTimer < myGrappleCooldown);
	
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGrapplePoint::StaticClass(), actors);

	const FVector forward = GetCamera().GetForwardVector();
	float dot = -1;
	AActor* ptr = nullptr;
	
	for(auto& it : actors)
	{
		auto diff = (it->GetTransform().GetTranslation() - GetActorTransform().GetTranslation());
		if(diff.Size() < myGrappleDist)
		{
			diff.Normalize();
			const auto currDot = FVector::DotProduct(forward, diff);
			if(currDot > myGrappleDot && (!ptr || currDot > dot))
			{
				ptr = it; 
				dot = currDot;
			}
		}
	}

	CHECK_RETURN(!ptr);
	myGrappleTargetLocation = ptr->GetTransform().GetTranslation();
	myGrappleTimer = 0.0f;
	auto& movement = GetCharacterMovement();	
	movement.AddImpulse(FVector(0, 0, movement.JumpZVelocity), true);
	GetAnimator().StartGrapple();
	LOG("Grapple");
}

void UFPMovement::Landed(const FHitResult& Hit)
{
	LOG("Landed");
	CreateEffect(myLandedEffect, GetActorTransform());
	myAirJumpCount = 0;
	if(myHoldingCrouch)
		StartCrouch(true);
	if (myHoldingJump)
		Jump();
}

void UFPMovement::StartWallrun(const FVector& aNormal)
{
	CHECK_RETURN(myHasHitHead);
	CHECK_RETURN(aNormal.Size() < 0.5f);

	if (!IsTouchingStickySurface())
	{
		LOG("Not touching sticy wall");
		return;
	}
	
	if (abs(aNormal.Z) > 0.5f)
	{
		LOG("Hit ceil or floor" + FString::SanitizeFloat(aNormal.Z));
		myHasHitHead = true;
		return;
	}

	auto& movement = GetCharacterMovement();
	
	if (myIsWallRunning)
	{
		const auto vel = movement.GetLastUpdateVelocity();
		const float speed = vel.Size();
		const auto projected = FVector::VectorPlaneProject(vel, aNormal);
		movement.Velocity = projected.GetSafeNormal() * speed;
	}

	movement.SetPlaneConstraintNormal(aNormal);
	myWallNormal = aNormal;
	
	if (myIsWallRunning)
		return;
	
	LOG("Starting wallrun with normal " + FString::SanitizeFloat(aNormal.Size()));	
	movement.SetPlaneConstraintEnabled(true);
	movement.GravityScale = 0.0f;
	myIsWallRunning = true;
	myWasWallRunning = true;
	myCoyoteTimer = 0.0f;
	myAirJumpCount = 0;
}

void UFPMovement::StopWallrun()
{
	GetAnimator().SetState(UFPAnimator::State::FALLING);
	auto& movement = GetCharacterMovement();
	movement.SetPlaneConstraintEnabled(false);
	movement.GravityScale = myOverrideGravityScale;
	myIsWallRunning = false;
	myCoyoteTimer = 0.0f;
	LOG("Stop wallrun");
}

void UFPMovement::Wallrun(const float aDT)
{
	const auto& character = GetCharacter();
	auto& movement = GetCharacterMovement();
	auto& animator = GetAnimator();
	
	if (myIsWallRunning)
	{
		const float lookForwardDot = FVector::DotProduct(myWallNormal, character.GetActorForwardVector());
		const float movementForwardDot = FVector::DotProduct(myWallNormal, character.GetLastMovementInputVector());

		// WallClimb
		if (lookForwardDot < -myWallClimbDot)
		{
			
			animator.SetState(UFPAnimator::State::WALL_CLIMBING);
			movement.SetPlaneConstraintEnabled(false);
			movement.Velocity = FVector(0, 0, myWallClimbSpeed);
			if (character.GetInputAxisValue("MoveVertical") < 0.1f)
			{
				LOG("Exited wallclimb");
				StopWallrun();
			}
		}
		else
		{
			animator.SetState(UFPAnimator::State::WALL_RUNNING);
			
			// Lim movement
			movement.SetPlaneConstraintEnabled(true);
			if (movement.Velocity.Z < 0.0f)
			{
				movement.Velocity.Z = 0.0f;
				movement.SetMovementMode(MOVE_Walking);
			}
			else movement.Velocity.Z *= FMath::Pow(0.1f, aDT);
		}

		// Stopping
		if (character.GetLastMovementInputVector().Size() < myWallrunMinVelocity)
		{
			LOG("Velocity too low");
			StopWallrun();
		}
		
		// Looking away
		if (lookForwardDot > myWallrunExitDot || movementForwardDot > myWallrunExitDot)
		{
			LOG("Looking away from wallrun");
			StopWallrun();
		}

		if (myHasHitHead)
		{
			LOG("Dont wallrun on floor or ceiling");
			StopWallrun();
		}

		if (!IsTouchingStickySurface())
		{
			LOG("Overlaps empty");
			StopWallrun();
		}
	}
	else if (movement.IsWalking() || myCoyoteTimer > myCoyoteTime)
	{
		myWasWallRunning = false;
		myHasHitHead = false;
	}
}

bool UFPMovement::IsTouchingStickySurface() const
{
	const auto capsule = GetCharacter().GetWallDetection();
	if (capsule)
	{
		auto overlapInfo = capsule->GetOverlapInfos();
		for (auto& it : overlapInfo)
			if(it.OverlapInfo.GetActor()->IsA(AStickySurface::StaticClass()))
				return true;
	}
	return false;
}

void UFPMovement::UpdateCrouch(const float aDT)
{
	CHECK_RETURN(!GetCharacter().bIsCrouched);
	
	if (myIsSliding)
	{
		auto& movement = GetCharacterMovement();
		mySlideTimer += aDT;
		if(mySlideTimer > mySlideDuration)
			StopSlide();

		const float movementDot = FVector::DotProduct(movement.GetLastInputVector(), movement.Velocity);
		if(movementDot < mySlideMinDot)
			StopSlide();
		
		movement.Velocity =
			mySlideDirection *
			movement.MaxWalkSpeed *
			mySlideSpeedMul;
	}
}

void UFPMovement::UpdateDash(const float aDT)
{
	auto& movement = GetCharacterMovement();
	if (movement.IsWalking())
		myHasDashed = false;
	
	const bool wasDashing = myDashTimer < myDashDuration;
	myDashTimer += aDT;
	const bool isDashing = myDashTimer < myDashDuration;
	if (wasDashing || isDashing)
		movement.Velocity = myDashDirection *
			((wasDashing && !isDashing) ? movement.MaxWalkSpeed : myDashVelocity);

	// Dash camera effect
	const float dashIn = myDashTimer * 1.5f / myDashDuration;
	const float dashOut = FMath::Clamp((myDashDuration - myDashTimer) / myDashDuration, 0.0f, 1.0f);
	const float dashForwardMul = FMath::Clamp(FVector::DotProduct(movement.GetLastUpdateVelocity(),
		GetCharacter().GetActorForwardVector()), -1.0f, 1.0f);
	GetCamera().AddAdditiveOffset(FTransform(),
		FMath::Min(dashIn, dashOut) * myDashFov * dashForwardMul);
}

void UFPMovement::UpdateGrapple(const float aDT)
{
	myGrappleTimer += aDT;
	CHECK_RETURN(myGrappleTimer > myGrappleReleaseTime);
	const auto diff = myGrappleTargetLocation - GetCharacter().GetActorLocation();
	if(diff.Size() > myGrappleReleaseDist)
		GetCharacterMovement().Velocity = diff * myGrappleSpeedMul;
}

void UFPMovement::StartCrouch(const bool aForceCrouch)
{
	const auto& movement = GetCharacterMovement();
	CHECK_RETURN((!aForceCrouch && !movement.IsWalking()))
	auto& character = GetCharacter();
	CHECK_RETURN(character.bIsCrouched)
	LOG("Start crouch");
	character.Crouch();
	//auto& animator = GetAnimator();
	//animator.MoveLeft(FVector(0, 0, 50));
	//animator.MoveRight(FVector(0, 0, 50));
	const auto vel = movement.GetLastUpdateVelocity(); 
	if (vel.Size2D() > movement.MaxWalkSpeedCrouched)
		StartSlide(vel);
}

void UFPMovement::StopCrouch() const
{
	auto& character = GetCharacter();
	CHECK_RETURN(!character.bIsCrouched)
	CHECK_RETURN(myIsSliding)
	LOG("Stop crouch");
	character.UnCrouch();
	auto& animator = GetAnimator(); 
	animator.MoveLeft(FVector(0, 0, -50));
	animator.MoveRight(FVector(0, 0, -50));
}

void UFPMovement::StartSlide(const FVector& aVelocity)
{
	LOG("Start slide");
	myIsSliding = true;
	mySlideTimer = 0.0f;
	mySlideDirection = aVelocity;
	mySlideDirection.Z = 0;
	mySlideDirection.Normalize();
	GetAnimator().SetState(UFPAnimator::State::IDLE);
}

void UFPMovement::StopSlide()
{
	myIsSliding = false;
	StopCrouch();
}

FVector UFPMovement::GetWallNormal() const
{
	return myWallNormal;
}
