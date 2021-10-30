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
	
	camera->ClearAdditiveOffset();

	myCoyoteTimer = myIsWallRunning || movement->IsWalking() ? 
		0.0f : myCoyoteTimer + DeltaTime;

	Wallrun(DeltaTime);

	if (movement->IsWalking())
		myHasDashed = false;
	else
		myDashTimer += DeltaTime;

	{
		// Dash camera effect
		const float dashIn = myDashTimer * 1.5f / myDashDuration;
		const float dashOut = FMath::Clamp((myDashDuration - myDashTimer) / myDashDuration, 0.0f, 1.0f);
		const float dashForwardMul = FMath::Clamp(FVector::DotProduct(movement->GetLastUpdateVelocity(),
			character->GetActorForwardVector()), -1.0f, 1.0f);
		camera->AddAdditiveOffset(FTransform(),
			FMath::Min(dashIn, dashOut) * myDashFov * dashForwardMul);
	}

	myGrappleTimer += DeltaTime;

	if(!myIsWallRunning)
	{
		if (movement->IsWalking())
			animator->SetState(movement->GetLastInputVector().Size() > 0.3f ? UFPAnimator::RUNNING : UFPAnimator::IDLE);
		else animator->SetState(UFPAnimator::FALLING);
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
		movement->Velocity = FVector();
		movement->AddImpulse(vel * myDashImpuleStrength, true);
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
		auto diff = (ptr->GetTransform().GetTranslation() - character->GetTransform().GetTranslation());
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Grapple dot " + FString::SanitizeFloat(dot) + " dist " + FString::SanitizeFloat(diff.Size()));
		auto cm = character->GetCharacterMovement();
		cm->Velocity = FVector();
		diff.Z *= myGrappleVerticalMul;
		diff *= myGrappleForceMul;
		cm->AddImpulse(diff, true);
		myGrappleTimer = 0.0f;

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
	
	movement->SetPlaneConstraintNormal(aNormal);
	myWallNormal = aNormal;
	if (myIsWallRunning)
	{
		// TODO: Transfer velocity
		return;
	}
	
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
		if ((lookForwardDot < -myWallClimbDot))
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
			if(!it.OverlapInfo.GetActor()->IsA(ASword::StaticClass()))
				noOverlaps = false;

		if (noOverlaps)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Overlaps empty");
			StopWallrun();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Overlaps");
		}
	}
	else if (movement->IsWalking() || myCoyoteTimer > myCoyoteTime)
	{
		myWasWallRunning = false;
		myHasHitHead = false;
	}
}

FVector UFPMovement::GetWallNormal() const
{
	return myWallNormal;
}