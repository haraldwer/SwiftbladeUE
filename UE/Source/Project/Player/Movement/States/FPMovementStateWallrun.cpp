#include "FPMovementStateWallrun.h"

#include "FPMovementStateDash.h"
#include "FPMovementStateInAir.h"
#include "FPMovementStateRun.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateWallclimb.h"
#include "Project/Player/Animation/States/FPAnimationStateWallrun.h"
#include "Project/Utility/Tools/CustomCamera.h"

void UFPMovementStateWallrun::Init()
{
	Super::Init();
	auto& movement = GetCharacterMovement();
	movement.GravityScale = myOverrideGravityScale;
}

UClass* UFPMovementStateWallrun::Update(float aDT)
{
	UpdateMovementNormal();
	myWallrunTimestamp = GetTime();

	auto& character = GetCharacter();
	auto& movement = GetCharacterMovement();
	auto& animator = GetAnimator();

	// Find normal and adjust distance
	float distance = 0.0f;
	if (FindWallInfo(myWallNormal, distance))
		character.SetActorLocation(character.GetActorLocation() - myWallNormal * (distance - myTargetDistance));
	
	const float lookForwardDot = FVector::DotProduct(myWallNormal, character.GetActorForwardVector());
	const float movementForwardDot = FVector::DotProduct(myWallNormal, character.GetLastMovementInputVector());
	
	if (lookForwardDot < -myWallClimbDot)
	{
		// Climbing
		
		animator.TryOverrideState(animator.GetState<UFPAnimationStateWallclimb>());
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
		// Wallrunning
		
		animator.TryOverrideState(animator.GetState<UFPAnimationStateWallrun>());

		// Limit max speed
		if (movement.Velocity.SizeSquared2D() > movement.MaxWalkSpeed)
			movement.Velocity = FMath::VInterpTo(
				movement.Velocity,
				movement.Velocity.GetSafeNormal() * movement.MaxWalkSpeed,
				aDT,
				myMaxSpeedSlowdown);
		
		if (movement.Velocity.Z < 0.0f)
		{
			movement.Velocity.Z = 0.0f;
			movement.SetMovementMode(MOVE_Walking);
		}
		else movement.Velocity.Z = FMath::FInterpTo(
			movement.Velocity.Z,
			0.0f,
			aDT,
			myWallrunGravity);
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

	// Hit head
	if (GetHitHead())
	{
		LOG("Hit head ray");
		return UFPMovementStateInAir::StaticClass();
	}
	
	return nullptr;
}

void UFPMovementStateWallrun::Enter()
{
	Super::Enter();
	UpdateMovementNormal();
	myWallrunTimestamp = GetTime();
	myHasWallJumped = false;
	LOG("Has Walljumped false");
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
	CHECK_RETURN(!IsCurrentState(), nullptr);
	return UFPMovementStateRun::StaticClass();
}

UClass* UFPMovementStateWallrun::OnHit(const FHitResult& aHit)
{
	CHECK_RETURN(aHit.ImpactNormal.Size() < 0.5f, nullptr);
	CHECK_RETURN_LOG(FMath::Abs(aHit.ImpactNormal.Z) > 0.5f, "Hit head normal", nullptr);
	CHECK_RETURN_LOG(GetHitHead(), "Hit head ray", nullptr)
	const auto& movement = GetCharacterMovement();
	CHECK_RETURN(movement.IsWalking(), nullptr);
	myWallNormal = aHit.Normal;
	float distance = 0.0f;
	FindWallInfo(myWallNormal, distance);
	return StaticClass();
}

bool UFPMovementStateWallrun::GetCanWallJump() const
{
	return !myHasWallJumped &&
		(GetCurrentState() == this ||
		(GetTime() - myWallrunTimestamp < myWallrunJumpCoyoteTime)); 
}

FVector UFPMovementStateWallrun::GetWalljumpDirection() const
{
	const auto normal2D = myWallNormal.GetSafeNormal2D();
	const float wallForwardDot = FVector::DotProduct(normal2D, GetCamera().GetForwardVector());
	LOG("Forward dot" + FString::SanitizeFloat(wallForwardDot));
	const float wallNormalPart = FMath::Lerp(1.0f, FMath::Abs(wallForwardDot), myWallrunJumpForwardPart);
	LOG("Normal part" + FString::SanitizeFloat(wallForwardDot));
	return FVector(0, 0, 1) + normal2D * myWallrunJumpMul * wallNormalPart;
}

void UFPMovementStateWallrun::OnWallJump()
{
	LOG("Has Walljumped true")
	myHasWallJumped = true;
}

void UFPMovementStateWallrun::UpdateMovementNormal() const
{
	auto& movement = GetCharacterMovement();
	const auto vel = movement.GetLastUpdateVelocity();
	const float speed = vel.Size();
	const auto normal2D = myWallNormal.GetSafeNormal2D();
	const auto projected = FVector::VectorPlaneProject(vel, normal2D);
	movement.Velocity = projected.GetSafeNormal() * speed;
	movement.GravityScale = 0.0f;
	movement.SetPlaneConstraintNormal(normal2D);
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

bool UFPMovementStateWallrun::GetHitHead() const
{
	// Check hit head
	const auto start = GetCharacter().GetActorLocation();
	const auto offset = FVector(0.0f, 0.0f, GetCharacter().GetDefaultHalfHeight() + 10.0f);
	const auto result = SingleRay(start, start + offset);
	return result.bBlockingHit; 
}

bool UFPMovementStateWallrun::FindWallInfo(FVector& aNormal, float& aDistance) const
{
	constexpr float depth = 100.0f;
	const auto normal = aNormal.GetSafeNormal2D();
	const FVector right = FVector::UpVector.Cross(normal);
	const FVector location = GetActorTransform().GetLocation();

	FVector normals = FVector::ZeroVector;
	float distance = 0.0f; 
	int32 hits = 0;

	// Brute force traces to find average normal
	static constexpr int numTraces = 20;
	for (int32 i = 0; i < numTraces; i++)
	{
		const float x = FMath::FRandRange(-myTestWidth * 0.5f, myTestWidth * 0.5f);
		const float y = FMath::FRandRange(-myTestHeight * 0.5f, myTestHeight * 0.5f);
		const FVector start = location + right * x + FVector::UpVector * y;
		const FVector end = start - normal * depth;
		const auto result = SingleRay(start, end);
		CHECK_CONTINUE(!result.bBlockingHit);
		CHECK_CONTINUE(FMath::Abs(result.Normal.Z) > 0.5f);
		hits++;
		normals += result.Normal.GetSafeNormal2D();
		distance += result.Distance;
	}

	// Average normal
	if (hits > 0)
	{
		aNormal = normals / static_cast<float>(hits);
		aDistance = distance / static_cast<float>(hits);
		return true;
	}

	return false;
}
