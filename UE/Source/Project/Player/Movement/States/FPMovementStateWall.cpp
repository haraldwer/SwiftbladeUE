#include "FPMovementStateWall.h"

#include "FPMovementStateDash.h"
#include "FPMovementStateInAir.h"
#include "FPMovementStateRun.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"
#include "Project/Gameplay/AbilityBlocker.h"
#include "Project/Gameplay/Sticky.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateWallclimb.h"
#include "Project/Player/Animation/States/FPAnimationStateWallrun.h"
#include "Project/Utility/Tools/CustomCamera.h"

void UFPMovementStateWall::Init()
{
	Super::Init();
	auto& movement = GetCharacterMovement();
	movement.GravityScale = myOverrideGravityScale;
}

UClass* UFPMovementStateWall::Update(float aDT)
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

	const auto inputVector = character.GetLastMovementInputVector();
	const float lookForwardDot = FVector::DotProduct(myWallNormal, character.GetActorForwardVector());
	const float movementForwardDot = FVector::DotProduct(myWallNormal, inputVector.GetSafeNormal2D());

	EFPMovementWallState state = RUN;
	if (movementForwardDot < -myWallSitMoveDot)
		state = (GetHitHead() || lookForwardDot > -myWallClimbLookDot) ? SIT : CLIMB;

	switch (state)
	{
	case SIT:
		{
			animator.TryOverrideState(animator.GetState<UFPAnimationStateWallrun>());

			// Limit max speed
			movement.Velocity = FMath::VInterpTo(
				movement.Velocity,
				FVector(0.0f, 0.0f, movement.Velocity.Z),
				aDT,
				myWallSitSlowdown);

			if (movement.Velocity.Z < 0.0f)
			{
				movement.Velocity.Z = 0.0f;
				movement.SetMovementMode(MOVE_Walking);
			}
			else movement.Velocity.Z = FMath::FInterpTo(
				movement.Velocity.Z,
				0.0f,
				aDT,
				myWallGravity);
		}
		break;

	case RUN:
		{
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
				myWallGravity);
		}
		break;
		
	case CLIMB:
		{
			animator.TryOverrideState(animator.GetState<UFPAnimationStateWallclimb>());
			movement.SetPlaneConstraintEnabled(false);
			movement.Velocity = FVector(0, 0, myWallClimbSpeed);
		}
		break;
	}
	
	// Stopping
	if (inputVector.Size() < myWallMinVelocity)
		return UFPMovementStateInAir::StaticClass();
		
	// Moving away
	if (movementForwardDot > myWallExitDot)
		return UFPMovementStateInAir::StaticClass();
	
	// No longer touching wall
	if (!GetIsOverlapping())
		return UFPMovementStateInAir::StaticClass();

	return nullptr;
}

void UFPMovementStateWall::Enter()
{
	Super::Enter();
	UpdateMovementNormal();
	myWallrunTimestamp = GetTime();
	myHasWallJumped = false;
	if (const auto dashState = GetState<UFPMovementStateDash>())
		dashState->Reset();
}

void UFPMovementStateWall::Exit()
{
	auto& movement = GetCharacterMovement();
	movement.SetPlaneConstraintEnabled(false);
	movement.GravityScale = myOverrideGravityScale;
	if (const auto airState = GetState<UFPMovementStateInAir>())
		airState->ResetJumps();
	Super::Exit();
}

UClass* UFPMovementStateWall::OnLanded()
{
	CHECK_RETURN(!IsCurrentState(), nullptr);
	return UFPMovementStateRun::StaticClass();
}

UClass* UFPMovementStateWall::OnHit(const FHitResult& aHit)
{
	CHECK_RETURN(!IsValidHit(aHit), nullptr);
	const auto& movement = GetCharacterMovement();
	CHECK_RETURN(movement.IsWalking(), nullptr);
	myWallNormal = aHit.Normal;
	float distance = 0.0f;
	FindWallInfo(myWallNormal, distance);
	return StaticClass();
}

bool UFPMovementStateWall::GetCanWallJump() const
{
	return !myHasWallJumped &&
		(GetCurrentState() == this ||
		(GetTime() - myWallrunTimestamp < myWallJumpCoyoteTime)); 
}

FVector UFPMovementStateWall::GetWalljumpDirection() const
{
	const auto normal2D = myWallNormal.GetSafeNormal2D();
	const float wallForwardDot = FVector::DotProduct(normal2D, GetCamera().GetForwardVector());
	const float wallNormalPart = FMath::Lerp(1.0f, FMath::Abs(wallForwardDot), myWallJumpForwardPart);
	return FVector(0, 0, 1) + normal2D * myWallJumpMul * wallNormalPart;
}

void UFPMovementStateWall::OnWallJump()
{
	myHasWallJumped = true;
}

void UFPMovementStateWall::UpdateMovementNormal() const
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

bool UFPMovementStateWall::GetIsOverlapping() const
{
	if (const auto capsule = GetCharacter().GetWallDetection())
	{
		const auto overlapInfo = capsule->GetOverlapInfos();
		for (auto& it : overlapInfo)
		{
			const auto actor = it.OverlapInfo.GetActor();
			CHECK_CONTINUE(!actor);
			CHECK_CONTINUE(actor == GetOwner())
			CHECK_CONTINUE(actor->IsA(ASword::StaticClass()));
			CHECK_CONTINUE(actor->IsA(AAbilityBlocker::StaticClass()));
			CHECK_CONTINUE(!actor->GetComponentByClass(USticky::StaticClass()));
			return true;
		}
	}
	return false;
}

bool UFPMovementStateWall::GetHitHead() const
{
	// Check hit head
	const auto start = GetCharacter().GetActorLocation();
	const auto offset = FVector(0.0f, 0.0f, GetCharacter().GetDefaultHalfHeight() + 10.0f);
	const auto result = Sweep(start, start + offset);
	return result.bBlockingHit; 
}

bool UFPMovementStateWall::FindWallInfo(FVector& aNormal, float& aDistance) const
{	
	const auto normal = aNormal.GetSafeNormal2D();
	const auto originalNormal = normal;
	const FVector right = FVector::UpVector.Cross(normal);
	const FVector location = GetActorTransform().GetLocation();

	FVector normals = FVector::ZeroVector;
	float totalNormals = 0.0f;
	float distance = 0.0f;
	int32 hits = 0;

	static constexpr int numTraces = 3;
	const float stepHeight = mySweepHeight / numTraces;
	const float startStep = mySweepHeight / 2.0f;

	TArray<TPair<FVector, FVector>> sweeps;
	sweeps.Reserve(numTraces * 3);
	
	for (int i = 0; i < numTraces; i++)
	{
		const FVector start = location + FVector::UpVector * (stepHeight - startStep);
		const FVector end = start - normal * mySweepDepth; 
		const FVector leftEnd = start - (normal - right).GetSafeNormal2D() * mySweepDepth; 
		const FVector rightEnd = start - (normal + right).GetSafeNormal2D() * mySweepDepth;
		sweeps.Add({start, end});
		sweeps.Add({start, leftEnd});
		sweeps.Add({start, rightEnd});	
	}

	for (auto& sweep : sweeps)
	{
		const auto result = Sweep(sweep.Key, sweep.Value, mySweepRadius);
		CHECK_CONTINUE(!IsValidHit(result));
		
		hits++;

		// Compare hit normal dot to in normal dot
		const auto resultNormal = result.Normal.GetSafeNormal2D();
		const float normalWeight = FMath::Max(0, resultNormal.Dot(originalNormal)); // Maybe dont do this? 
		normals += resultNormal * normalWeight;
		distance += result.Distance * normalWeight;
		totalNormals += normalWeight;
	}

	// Average normal and distance
	if (hits > 0)
	{
		aNormal = (normals / totalNormals).GetSafeNormal2D();
		aDistance = distance / totalNormals;
		return true;
	}

	return false;
}

bool UFPMovementStateWall::IsValidHit(const FHitResult& aHit)
{
	CHECK_RETURN(!aHit.bBlockingHit, false);
	CHECK_RETURN(FMath::Abs(aHit.Normal.Z) > 0.5f, false);

	// Require overlap events
	const auto comp = aHit.GetComponent();
	CHECK_RETURN(!comp, false);
	CHECK_RETURN(!comp->GetGenerateOverlapEvents(), false);
		
	const auto actor = aHit.GetActor();
	CHECK_RETURN(!actor, false);
	CHECK_RETURN(actor->IsA(AAbilityBlocker::StaticClass()), false);

	// Is sticky? 
	CHECK_RETURN(!actor->GetComponentByClass(USticky::StaticClass()), false);

	return true; 
}
