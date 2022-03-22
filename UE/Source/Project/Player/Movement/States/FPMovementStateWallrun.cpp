#include "FPMovementStateWallrun.h"

#include "FPMovementStateDash.h"
#include "FPMovementStateInAir.h"
#include "FPMovementStateRun.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Animation/FPAnimator.h"

void UFPMovementStateWallrun::Init()
{
	Super::Init();
	auto& movement = GetCharacterMovement();
	movement.GravityScale = myOverrideGravityScale;
}

UClass* UFPMovementStateWallrun::Update(float aDT)
{
	UpdateWallNormal();
	myWallrunTimestamp = GetTime();

	// Set animator
	const auto& character = GetCharacter();
	auto& movement = GetCharacterMovement();
	auto& animator = GetAnimator();
	
	const float lookForwardDot = FVector::DotProduct(myWallNormal, character.GetActorForwardVector());
	const float movementForwardDot = FVector::DotProduct(myWallNormal, character.GetLastMovementInputVector());
	
	if (lookForwardDot < -myWallClimbDot)
	{
		animator.SetState(UFPAnimator::State::WALL_CLIMBING);
		movement.SetPlaneConstraintEnabled(false);
		movement.Velocity = FVector(0, 0, myWallClimbSpeed);
		if (character.GetInputAxisValue("MoveVertical") < 0.1f)
		{
			LOG("No forward input when climbing");
			return UFPMovementStateInAir::StaticClass();
		}
	}
	else
	{
		animator.SetState(UFPAnimator::State::WALL_RUNNING);
		if (movement.Velocity.Z < 0.0f)
		{
			movement.Velocity.Z = 0.0f;
			movement.SetMovementMode(MOVE_Walking);
		}
		else movement.Velocity.Z *= FMath::Pow(myWallrunGravity, aDT);
	}
	
	// Stopping
	if (character.GetLastMovementInputVector().Size() < myWallrunMinVelocity)
	{
		LOG("Stopping");
		return UFPMovementStateInAir::StaticClass();
	}
		
	// Looking away
	if (lookForwardDot > myWallrunExitDot || movementForwardDot > myWallrunExitDot)
	{
		LOG("Looking away");
		return UFPMovementStateInAir::StaticClass();
	}
	
	// No longer touching wall
	if (!GetIsOverlapping())
	{
		LOG("No longer touching wall");
		return UFPMovementStateInAir::StaticClass();
	}
	
	return nullptr;
}

void UFPMovementStateWallrun::Enter()
{
	Super::Enter();
	UpdateWallNormal();
	myWallrunTimestamp = GetTime();
	if (const auto dashState = GetState<UFPMovementStateDash>())
		dashState->Reset();
}

void UFPMovementStateWallrun::Exit()
{
	auto& movement = GetCharacterMovement();
	movement.SetPlaneConstraintEnabled(false);
	movement.GravityScale = myOverrideGravityScale;
	if (const auto airState = GetState<UFPMovementStateInAir>())
		airState->ResetJumps();
	Super::Exit();
}

UClass* UFPMovementStateWallrun::OnLanded()
{
	myHasHitHead = false;
	CHECK_RETURN(!IsCurrentState(), nullptr);
	return UFPMovementStateRun::StaticClass();
}

UClass* UFPMovementStateWallrun::OnHit(const FHitResult& aHit)
{
	CHECK_RETURN(aHit.ImpactNormal.Size() < 0.5f, nullptr);
	CHECK_RETURN_LOG(myHasHitHead, "Has hit head, blocking", nullptr);
	myWallNormal = aHit.ImpactNormal;

	if (IsCurrentState())
	{
		if (abs(aHit.ImpactNormal.Z) > 0.5f)
		{
			LOG("Hit ceil or floor" + FString::SanitizeFloat(aHit.ImpactNormal.Z));
			myHasHitHead = true;
			return UFPMovementStateInAir::StaticClass();
		}
		return nullptr;
	}
	
	const auto& movement = GetCharacterMovement();
	CHECK_RETURN(movement.IsWalking(), nullptr);
	return StaticClass();
}

bool UFPMovementStateWallrun::GetCanWallJump() const
{
	return GetCurrentState() == this ||
		(GetTime() - myWallrunTimestamp < myWallrunJumpCoyoteTime); 
}

FVector UFPMovementStateWallrun::GetWalljumpDirection() const
{
	return FVector(0, 0, 1) + myWallNormal * myWallrunJumpMul * 
		(1 - FMath::Abs(FVector::DotProduct(myWallNormal, GetCamera().GetForwardVector())));
}

void UFPMovementStateWallrun::UpdateWallNormal() const
{
	auto& movement = GetCharacterMovement();
	const auto vel = movement.GetLastUpdateVelocity();
	const float speed = vel.Size();
	const auto projected = FVector::VectorPlaneProject(vel, myWallNormal);
	movement.Velocity = projected.GetSafeNormal() * speed;
	movement.GravityScale = 0.0f;
	movement.SetPlaneConstraintNormal(myWallNormal);
	movement.SetPlaneConstraintEnabled(true);
}

bool UFPMovementStateWallrun::GetIsOverlapping() const
{
	if (const auto capsule = GetCharacter().GetWallDetection())
	{
		const auto overlapInfo = capsule->GetOverlapInfos();
		for (auto& it : overlapInfo)
		{
			if(it.OverlapInfo.GetActor() == GetOwner())
				continue;

			if(it.OverlapInfo.GetActor()->IsA(ASword::StaticClass()))
				continue;

			return true;
		}
	}
	return false;
}
