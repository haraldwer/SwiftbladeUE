#include "FPMovement.h"
#include "FPCharacter.h"
#include "Actors/Sword.h"
#include "Animation/FPAnimator.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Gameplay/GrapplePoint.h"

UFPMovement::UFPMovement()
{
	PrimaryComponentTick.bCanEverTick = true;

	const auto character = GetFPCharacter();
	if (!character)
		return;
	
	const auto capsule = character->GetCapsuleComponent();
	if(capsule)
	{
		myWallDetection = CreateDefaultSubobject<UCapsuleComponent>("WallDetection");
		check(myWallDetection != nullptr);
		float radius, halfHeight;
		capsule->GetUnscaledCapsuleSize(radius, halfHeight);
		myWallDetection->InitCapsuleSize(radius + 10, halfHeight + 10);
		myWallDetection->SetupAttachment(GetFPCharacter()->GetRootComponent());
		myWallDetection->SetGenerateOverlapEvents(true);
	}	
}

void UFPMovement::BeginPlay()
{
	Super::BeginPlay();

	const auto character = GetFPCharacter();
	if (!character)
		return;
	
	const auto camera = character->GetCamera();
	const auto capsule = character->GetCapsuleComponent();
	if (camera && capsule)
	{
		myCameraHeight = camera->GetRelativeLocation().Z + capsule->GetScaledCapsuleHalfHeight();
		myOriginalCameraHeight = myCameraHeight;
	}
}

void UFPMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;

	const auto camera = character->GetCamera();
	if (!camera)
		return;

	const auto animator = character->GetAnimator();
	if (!animator)
		return;

	myCoyoteTimer = myIsWallRunning || movement->IsWalking() ? 
		0.0f : myCoyoteTimer + DeltaTime;

	UpdateCrouch(DeltaTime);
	Wallrun(DeltaTime);
	UpdateDash(DeltaTime);
	UpdateGrapple(DeltaTime);

	if (!myIsWallRunning && !myIsSliding)
	{
		if (movement->IsWalking())
			animator->SetState(movement->GetLastInputVector().Size() > 0.3f ? UFPAnimator::RUNNING : UFPAnimator::IDLE);
		else animator->SetState(UFPAnimator::FALLING);
	}

	// Update camera height
	{
		const float targetCamHeight = myOriginalCameraHeight * 
			(movement->IsCrouching() ? myCrouchCamHeightMul : 1.0f);
		
		myCameraHeight = FMath::FInterpTo(
			myCameraHeight,
			targetCamHeight,
			DeltaTime,
			myCameraHeightSmoothing);

		const auto capsule = character->GetCapsuleComponent();
		if(capsule)
		{
			const float height = myCameraHeight - capsule->GetScaledCapsuleHalfHeight();
			// Feet location + height
			camera->SetRelativeLocation(FVector(0, 0, height));
		}
	}
}

AFPCharacter* UFPMovement::GetFPCharacter() const
{
	return Cast<AFPCharacter>(GetOwner());
}

void UFPMovement::PressJump()
{
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
	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;

	if (!movement->IsWalking())
		return;

	if (character->bIsCrouched)
		return;
	
	character->Crouch();
	
	const auto vel = movement->GetLastUpdateVelocity(); 
	if (vel.Size2D() > movement->MaxWalkSpeedCrouched)
		StartSlide(vel);
}

void UFPMovement::ReleaseCrouch()
{
	const auto character = GetFPCharacter();
	if (!character)
		return;

	if (!character->bIsCrouched)
		return;

	if (myIsSliding)
		return;
	
	character->UnCrouch();
}

void UFPMovement::Jump()
{
	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;

	const auto camera = character->GetCamera();
	if (!camera)
		return;

	const auto animator = character->GetAnimator();
	if (!animator)
		return;
	
	const bool onGround = myIsWallRunning || movement->IsWalking() || myCoyoteTimer < myCoyoteTime;
	if(onGround || myAirJumpCount < myNumAirJumps)
	{
		FVector direction = FVector(0, 0, 1);
		if (myWasWallRunning) direction += myWallNormal * myWallrunJumpMul * 
			(1 - FMath::Abs(FVector::DotProduct(myWallNormal, camera->GetForwardVector())));
		movement->Velocity.Z = 0;
		movement->AddImpulse(direction * movement->JumpZVelocity, true);
		if(!onGround)
			myAirJumpCount++;
		animator->StartJump();
		StopSlide();
	}
}

void UFPMovement::Dash()
{
	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;

	const auto camera = character->GetCamera();
	if (!camera)
		return;

	if(!myHasDashed)
	{
		auto vel = camera->GetForwardVector();
		if (movement->IsWalking() && vel.Z < 0.1f)
			vel.Z = 0.1f;
		const auto input = character->GetLastMovementInputVector();
		if(input.Size() > 0.3f)
			vel = FVector(input.X, input.Y, vel.Z);
		vel.Normalize();
		vel.Z += 0.1f * (1 - abs(vel.Z));
		vel.Z *= myDashVerticalMul;

		myDashDirection = vel;
		movement->Velocity = vel * myDashVelocity;
		myHasDashed = true;
		myDashTimer = 0;
		
		const auto animator = character->GetAnimator();
		if (animator)
			animator->StartDash();
	}
}


void UFPMovement::Grapple()
{
	if (myGrappleTimer < myGrappleCooldown)
		return;

	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto camera = character->GetCamera();
	if (!camera)
		return;
	
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGrapplePoint::StaticClass(), actors);

	const FVector forward = camera->GetForwardVector();
	float dot = -1;
	AActor* ptr = nullptr;
	
	for(auto& it : actors)
	{
		auto diff = (it->GetTransform().GetTranslation() - character->GetTransform().GetTranslation());
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

	if(ptr)
	{
		myGrappleTargetLocation = ptr->GetTransform().GetTranslation();
		myGrappleTimer = 0.0f;
		
		const auto movement = character->GetCharacterMovement();		
		movement->AddImpulse(FVector(0, 0, movement->JumpZVelocity), true);
		
		const auto animator = character->GetAnimator();
		if (animator)
			animator->StartGrapple();
	}
}

void UFPMovement::Landed(const FHitResult& Hit)
{
	myAirJumpCount = 0;
	if (myHoldingJump)
		Jump();
}

void UFPMovement::StartWallrun(FVector aNormal)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Hit wall");
	
	if (myHasHitHead)
		return;
	
	if (aNormal.Size() < 0.5f)
		return;
	
	if (abs(aNormal.Z) > 0.5f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Hit ceil or floor" + FString::SanitizeFloat(aNormal.Z));
		myHasHitHead = true;
		return;
	}

	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;
	
	if (myIsWallRunning)
	{
		const auto vel = movement->GetLastUpdateVelocity();
		const float speed = vel.Size();
		const auto projected = FVector::VectorPlaneProject(vel, aNormal);
		movement->Velocity = projected.GetSafeNormal() * speed;
	}

	movement->SetPlaneConstraintNormal(aNormal);
	myWallNormal = aNormal;
	
	if (myIsWallRunning)
		return;
	
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Starting wallrun with normal " + FString::SanitizeFloat(aNormal.Size()));	
	movement->SetPlaneConstraintEnabled(true);
	movement->GravityScale = 0.0f;
	myIsWallRunning = true;
	myWasWallRunning = true;
	myCoyoteTimer = 0.0f;
	myAirJumpCount = 0;
}

void UFPMovement::StopWallrun()
{
	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;

	const auto animator = character->GetAnimator();
	if(animator)
		animator->SetState(UFPAnimator::FALLING);
	
	movement->SetPlaneConstraintEnabled(false);
	movement->GravityScale = 1.0f;
	myIsWallRunning = false;
	myCoyoteTimer = 0.0f;
}

void UFPMovement::Wallrun(float aDT)
{
	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;

	const auto animator = character->GetAnimator();
	
	if (myIsWallRunning)
	{
		const float lookForwardDot = FVector::DotProduct(myWallNormal, character->GetActorForwardVector());
		const float movementForwardDot = FVector::DotProduct(myWallNormal, character->GetLastMovementInputVector());

		// WallClimb
		if (lookForwardDot < -myWallClimbDot)
		{
			if(animator)
				animator->SetState(UFPAnimator::WALL_CLIMBING);
			movement->SetPlaneConstraintEnabled(false);
			movement->Velocity = FVector(0, 0, myWallClimbSpeed);
			if (character->GetInputAxisValue("MoveVertical") < 0.1f)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Exited wallclimb");
				StopWallrun();
			}
		}
		else
		{
			if(animator)
				animator->SetState(UFPAnimator::WALL_RUNNING);
			
			// Lim movement
			movement->SetPlaneConstraintEnabled(true);
			if (movement->Velocity.Z < 0.0f)
			{
				movement->Velocity.Z = 0.0f;
				movement->SetMovementMode(MOVE_Walking);
			}
			else movement->Velocity.Z *= FMath::Pow(0.1f, aDT);
		}

		// Stopping
		if (character->GetLastMovementInputVector().Size() < myWallrunMinVelocity)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Velocity too low");
			StopWallrun();
		}
		
		// Looking away
		if (lookForwardDot > myWallrunExitDot || movementForwardDot > myWallrunExitDot)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Looking away from wallrun");
			StopWallrun();
		}

		if (myHasHitHead)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Dont wallrun on floor or ceiling");
			StopWallrun();
		}

		bool noOverlaps = true;
		auto overlapInfo = myWallDetection->GetOverlapInfos();
		for (auto& it : overlapInfo)
		{
			if(it.OverlapInfo.GetActor() == GetOwner())
				continue;

			if(it.OverlapInfo.GetActor()->IsA(ASword::StaticClass()))
				continue;
			
			noOverlaps = false;
		}

		if (noOverlaps)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Overlaps empty");
			StopWallrun();
		}
	}
	else if (movement->IsWalking() || myCoyoteTimer > myCoyoteTime)
	{
		myWasWallRunning = false;
		myHasHitHead = false;
	}
}

void UFPMovement::UpdateCrouch(float aDT)
{
	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;

	const auto camera = character->GetCamera();
	if (!camera)
		return;

	if (!character->bIsCrouched)
		return;
	
	if (myIsSliding)
	{
		mySlideTimer += aDT;
		if(mySlideTimer > mySlideDuration)
			StopSlide();

		const float movementDot = FVector::DotProduct(movement->GetLastInputVector(), movement->Velocity);
		if(movementDot < mySlideMinDot)
			StopSlide();
		
		movement->Velocity =
			mySlideDirection *
			movement->MaxWalkSpeed *
			mySlideSpeedMul;
	}
}

void UFPMovement::UpdateDash(float aDT)
{
	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;

	const auto camera = character->GetCamera();
	if (!camera)
		return;
	
	if (movement->IsWalking())
		myHasDashed = false;
	
	const bool wasDashing = myDashTimer < myDashDuration;
	myDashTimer += aDT;
	const bool isDashing = myDashTimer < myDashDuration;
	if (wasDashing || isDashing)
		movement->Velocity = myDashDirection *
			((wasDashing && !isDashing) ? movement->MaxWalkSpeed : myDashVelocity);

	// Dash camera effect
	const float dashIn = myDashTimer * 1.5f / myDashDuration;
	const float dashOut = FMath::Clamp((myDashDuration - myDashTimer) / myDashDuration, 0.0f, 1.0f);
	const float dashForwardMul = FMath::Clamp(FVector::DotProduct(movement->GetLastUpdateVelocity(),
		character->GetActorForwardVector()), -1.0f, 1.0f);
	camera->AddAdditiveOffset(FTransform(),
		FMath::Min(dashIn, dashOut) * myDashFov * dashForwardMul);
}

void UFPMovement::UpdateGrapple(float aDT)
{
	myGrappleTimer += aDT;

	if(myGrappleTimer > myGrappleReleaseTime)
		return;
	
	const auto character = GetFPCharacter();
	if (!character)
		return;

	const auto movement = character->GetCharacterMovement();
	if (!movement)
		return;
	
	const auto diff = myGrappleTargetLocation - character->GetActorLocation();
	if(diff.Size() > myGrappleReleaseDist)
		movement->Velocity = diff * myGrappleSpeedMul;
}

void UFPMovement::StartSlide(FVector aVelocity)
{
	myIsSliding = true;
	mySlideTimer = 0.0f;
	mySlideDirection = aVelocity;
	mySlideDirection.Z = 0;
	mySlideDirection.Normalize();

	const auto character = GetFPCharacter();
	if (!character)
		return;
	
	const auto animator = character->GetAnimator();
	if (!animator)
		return;

	animator->SetState(UFPAnimator::IDLE);
}

void UFPMovement::StopSlide()
{
	myIsSliding = false;
	ReleaseCrouch();
}

FVector UFPMovement::GetWallNormal() const
{
	return myWallNormal;
}
