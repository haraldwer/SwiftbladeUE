#include "FPMovementStateVault.h"

#include "FPMovementStateCrouch.h"
#include "FPMovementStateInAir.h"
#include "FPMovementStateWall.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Gameplay/Abilities/Sticky.h"
#include "Project/Player/FPCamera.h"
#include "Project/Player/FPCharacter.h"

UClass* UFPMovementStateVault::Update(float aDT)
{
	const auto& character = GetCharacter(); 
	auto& movement = GetCharacterMovement();

	// Is still looking and moving towards vault? 
	const auto forward = character.GetActorForwardVector();
	const auto input = movement.GetLastInputVector();
	CHECK_RETURN(forward.Dot(myInitialForward) < myInputForwardMinDot, UFPMovementStateInAir::StaticClass());
	CHECK_RETURN(input.Dot(myInitialForward) < myInputForwardMinDot, UFPMovementStateInAir::StaticClass());

	// Crouch camera
	if (myCrouch)
	{
		auto& fpCamera = GetFPCamera();
		fpCamera.SetTargetHeight(myCamHeightMul);
	}
	
	const float currentHeight = character.GetActorLocation().Z;
	const float targetHeight = myHit.Location.Z + character.GetSimpleCollisionHalfHeight();
	if (currentHeight < targetHeight)
	{
		// Vertical velocity
		if (movement.Velocity.Z < myUpwardsVelocity)
			movement.Velocity.Z = FMath::FInterpTo(
				movement.Velocity.Z,
				myUpwardsVelocity,
				aDT,
				myUpwardsSpeedup);
		myUpwardTimestamp = GetCurrentTime();
	}
	else
	{
		// Flatten velocity upwards
		if (movement.Velocity.Z > 0.0f)
			movement.Velocity.Z = FMath::FInterpTo(
				movement.Velocity.Z,
				0.0f,
				aDT,
				myUpwardsSlowdown);

		// Recover forward velocity 
		const FVector flatVel = movement.Velocity * FVector(1.0f, 1.0f, 0.0f);
		const float interpSpeed = FMath::FInterpTo(
			flatVel.Size(),
			movement.MaxWalkSpeed,
			aDT,
			myFlatVelRecorverySpeed);
		const FVector interpVel = input.GetSafeNormal2D() * interpSpeed; 
		movement.Velocity = interpVel + FVector::UpVector * movement.Velocity.Z; 
		
		const float timeDiff = GetCurrentTime() - myUpwardTimestamp;
		if (timeDiff > myExitDelay)
			return UFPMovementStateInAir::StaticClass();
	}
	
	return StaticClass();
}

UClass* UFPMovementStateVault::Check()
{
	const auto currState = GetCurrentState();
	CHECK_RETURN(!currState, nullptr);
	CHECK_RETURN(!currState->IsA(UFPMovementStateInAir::StaticClass()), nullptr);
	return CheckConditions(); 
}

UClass* UFPMovementStateVault::Input(EFPMovementInputAction anAction, float aValue)
{
	Super::Input(anAction, aValue);
	return CheckConditions(); 
}

void UFPMovementStateVault::Enter()
{
	auto& character = GetCharacter();
	if (myCrouch)
		character.Crouch();
	myInitialForward = character.GetActorForwardVector();
	myUpwardTimestamp = GetCurrentTime();
	
	LOG("Enter");
}

void UFPMovementStateVault::Exit()
{
	auto& movement = GetCharacterMovement();
	if (movement.Velocity.Z > myMaxExitVerticalSpeed)
		movement.Velocity.Z = myMaxExitVerticalSpeed;
	
	if (myCrouch)
	{
		auto& character = GetCharacter();
		character.UnCrouch();
	}
	
	LOG("Exit");
}

TSubclassOf<UFPAnimationStateBase> UFPMovementStateVault::GetAnimation() const
{
	return {};
}

UClass* UFPMovementStateVault::CheckConditions()
{
	// Check movement vs character dir
	const auto forward = GetCharacter().GetActorForwardVector();
	const auto input = GetCharacterMovement().GetLastInputVector();
	CHECK_RETURN(forward.Dot(input) < myInputForwardMinDot, nullptr);
	
	const auto hit = Sweep();
	CHECK_RETURN(!hit.bBlockingHit, nullptr);

	myHit = hit; 
	return StaticClass();
}

FHitResult UFPMovementStateVault::Sweep() const
{
	auto& character = GetCharacter();
	const auto loc = character.GetActorLocation();
	const auto forward = character.GetActorForwardVector();
	const FVector start = loc + forward * mySweepForwardOffset + FVector::UpVector * mySweepStartZ;   
	const FVector end = loc + forward * mySweepForwardOffset + FVector::UpVector * mySweepEndZ;
	
	TArray<FHitResult> hits;
	FCollisionQueryParams params;
	params.bFindInitialOverlaps = false; 
	if (GetWorld()->SweepMultiByChannel(hits, start, end, FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeSphere(mySweepRadius), params))
	{
		CHECK_RETURN(!hits.Num(), {});

		FHitResult total;
		float totalWeight = 0.0f;
		FVector normal = FVector::ZeroVector;
		FVector location = FVector::ZeroVector; 
		for (auto& hit : hits)
		{
			CHECK_CONTINUE(!hit.bBlockingHit);

			// Filter hits
			if (auto hitActor = hit.GetActor())
				if (hitActor->IsA(AEnemy::StaticClass()) ||
					hitActor->FindComponentByClass(USticky::StaticClass()))
					continue; 

			if (!total.bBlockingHit)
				total = hit;  
			const float weight = hit.Normal.Z; 
			totalWeight += weight; 
			normal += hit.Normal * weight; 
			location += hit.Location * weight;
		}
		
		if (totalWeight > 0.0f)
		{
			total.Normal = normal / totalWeight;
			total.Location = location / totalWeight;
		}
		return total; 
	}
	
	return FHitResult(); 
}
