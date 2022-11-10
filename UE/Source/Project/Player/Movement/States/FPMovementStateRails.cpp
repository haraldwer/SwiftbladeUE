#include "FPMovementStateRails.h"

#include "FPMovementStateDash.h"
#include "FPMovementStateInAir.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Gameplay/RailComponent.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Player/Animation/States/FPAnimationStateRails.h"
#include "Project/Player/Movement/FPMovement.h"

UClass* UFPMovementStateRails::Update(float aDT)
{
	const auto rail = myRail.Get();
	CHECK_RETURN_LOG(!rail, "Invalid rail", UFPMovementStateInAir::StaticClass());
	const auto spline = rail->GetSpline();
	CHECK_RETURN_LOG(!spline, "Invalid spline", UFPMovementStateInAir::StaticClass());
	
	auto& movement = GetCharacterMovement();
	auto& character = GetCharacter();
	const FVector input = (movement.GetLastInputVector() * FVector(1.0f, 1.0f, 0.0f)).GetSafeNormal();
	
	if (mySplineDist < 0.0f)
	{
		
		const FVector worldLoc = GetActorTransform().GetLocation() + GetCharacter().GetDefaultHalfHeight();
		const FVector velOff = movement.Velocity * myInitialVelOffsetMul;
		const float key = spline->FindInputKeyClosestToWorldLocation(worldLoc + velOff);
		mySplineDist = spline->GetDistanceAlongSplineAtSplineInputKey(key);
		myInterpInput = input; 
	}

	const FVector tangent = spline->GetTangentAtDistanceAlongSpline(mySplineDist, ESplineCoordinateSpace::World).GetSafeNormal();

	myInterpInput = FMath::VInterpTo(myInterpInput, input, aDT, myInputInterpSpeed);
	const float dot = FVector::DotProduct(myInterpInput.GetSafeNormal(), tangent) * 2.0f * myInterpInput.Length();
	const float splineDot = FMath::Clamp(dot, -1.0f, 1.0f);
	
	mySplineDist += myRailVelocity * aDT * splineDot;
	CHECK_RETURN(mySplineDist > spline->GetSplineLength() || mySplineDist < 0.0f, UFPMovementStateInAir::StaticClass());
	
	const FVector adjustedLoc = spline->GetLocationAtDistanceAlongSpline(mySplineDist, ESplineCoordinateSpace::World);
	const FVector desiredLoc = adjustedLoc + FVector::UpVector * myVerticalOffset;
	const FVector currLoc = character.GetActorLocation();
	const FVector newLoc = FMath::VInterpTo(currLoc, desiredLoc, aDT, myInterpSpeed);
	
	character.SetActorLocation(newLoc);
	const FVector deltaLoc = newLoc - currLoc;  
	movement.Velocity = deltaLoc * (1.0f / aDT);
	
	return StaticClass();
}

UClass* UFPMovementStateRails::Check()
{
	// Check overlaps
	TArray<AActor*> overlaps;
	GetCharacter().GetOverlappingActors(overlaps);
	for (const auto& overlap : overlaps)
	{
		TArray<URailComponent*> rails;
		overlap->GetComponents<URailComponent>(rails);
		for (const auto& rail : rails)
		{
			if (rail == myPrevRail.Get())
				if (GetCurrentTime() - myExitTimestamp < myCooldown)
					continue;
			
			myRail = rail;
			return StaticClass();
		}
	}
	return nullptr;
}

UClass* UFPMovementStateRails::Input(const EFPMovementInputAction anAction, const float aValue)
{
	Super::Input(anAction, aValue);
	return nullptr;
}

void UFPMovementStateRails::Enter()
{
	GetCharacterMovement().SetMovementMode(MOVE_Falling);
	ResetAbilities();
}

void UFPMovementStateRails::Exit()
{
	// Retain velocity?

	if (const auto airState = GetState<UFPMovementStateInAir>())
		airState->ResetJumps();

	myPrevRail = myRail;
	myRail.Reset();
	myExitTimestamp = GetCurrentTime();
	mySplineDist = -1.0f; 
}

TSubclassOf<UFPAnimationStateBase> UFPMovementStateRails::GetAnimation() const
{
	return UFPAnimationStateRails::StaticClass();
}

FTransform UFPMovementStateRails::GetSplineTransform() const
{
	const auto rail = myRail.Get();
	CHECK_RETURN_LOG(!rail, "Invalid rail", FTransform::Identity);
	const auto spline = rail->GetSpline();
	CHECK_RETURN_LOG(!spline, "Invalid spline", FTransform::Identity);
	return spline->GetTransformAtDistanceAlongSpline(mySplineDist, ESplineCoordinateSpace::World);
}
