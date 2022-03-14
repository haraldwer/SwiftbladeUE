#include "FPAnimator.h"

#include "DrawDebugHelpers.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Actors/Hand.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/LineBatchComponent.h"
#include "Project/Player/FPCamera.h"
#include "Project/Player/FPCombat.h"
#include "Project/Player/Actors/Interaction.h"
#include "Project/Player/Movement/FPMovementStateMachine.h"
#include "Project/Player/Movement/States/FPMovementStateWallrun.h"

UFPAnimator::UFPAnimator()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFPAnimator::BeginPlay()
{
	Super::BeginPlay();
	myState = State::IDLE;
}

void UFPAnimator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (myState)
	{
	case State::IDLE:			Idle(DeltaTime);		break;
	case State::RUNNING:		Running(DeltaTime);		break;
	case State::WALL_RUNNING:	WallRunning(DeltaTime);	break;
	case State::WALL_CLIMBING:	WallClimbing(DeltaTime);break;
	case State::FALLING:		Falling(DeltaTime);		break;
	default:			myState = State::IDLE;
	}

	// Apply overrides here
	Interact(DeltaTime);
	Jump(DeltaTime);
	Dash(DeltaTime);
	Grapple(DeltaTime);
	
	myRealHeadPosition = FMath::FInterpTo(myRealHeadPosition, myHeadPosition, DeltaTime, mySmoothing);
	myRealHeadRotation = FMath::FInterpTo(myRealHeadRotation, myHeadRotation, DeltaTime, mySmoothing);

	auto& camera = GetFPCamera();
	camera.SetOffset(FTransform(
		FQuat(FVector(1, 0, 0), myRealHeadRotation), // Rotation
			FVector(0, 0, myRealHeadPosition))); // Position

	myRealRight.SetRotation(
		FMath::QInterpTo(
			myRealRight.GetRotation(),
			myRight.GetRotation(), 
			DeltaTime, myHandSmoothing).GetNormalized());
	myRealRight.SetLocation(
		FMath::VInterpTo(
			myRealRight.GetLocation(),
			myRight.GetLocation(), 
			DeltaTime, myHandSmoothing));

	myRealLeft.SetRotation(
		FMath::QInterpTo(
			myRealLeft.GetRotation(),
			myLeft.GetRotation(),
			DeltaTime, myHandSmoothing).GetNormalized());
	myRealLeft.SetLocation(
		FMath::VInterpTo(
			myRealLeft.GetLocation(),
			myLeft.GetLocation(),
			DeltaTime, myHandSmoothing));

	if (!myRealRight.IsValid())
	{
		LOG("Animator right not valid");
	}
	
	if (!myRealLeft.IsValid())
	{
		LOG("Animator left not valid");
	}
}

void UFPAnimator::SetState(State aState)
{
	myState = aState;
}

void UFPAnimator::StartDash()
{
	myDashTime = 0;
}

void UFPAnimator::StartJump()
{
	myJumpTime = 0;
}

void UFPAnimator::StartGrapple()
{
	myGrappleTime = 0;
}

FTransform UFPAnimator::GetRight() const
{
	return myRealRight;
}

FTransform UFPAnimator::GetLeft() const
{
	return myRealLeft;
}

float UFPAnimator::GetSwordPart() const
{
	return mySwordPart;
}

void UFPAnimator::MoveLeft(const FVector& aVector)
{
	myRealLeft.SetLocation(myRealLeft.GetLocation() + aVector);
}

void UFPAnimator::MoveRight(const FVector& aVector)
{
	myRealRight.SetLocation(myRealRight.GetLocation() + aVector);
}

void UFPAnimator::Idle(float aDT)
{
	const auto& character = GetCharacter();
	const auto& camera = GetCamera();
	
	myTimer += aDT * mySpeed * myIdleSpeed;
	const float off = FMath::Sin(myTimer);
	myHeadPosition = FMath::Abs(off) * myPositionStrength * myIdleScale;
	myHeadRotation = 0.0f;

	const float upDot = FMath::Clamp(FVector::DotProduct(
		                                 character.GetActorUpVector(), camera.GetForwardVector()), 0.0f, 1.0f);
	
	FVector look;
	look.Z = upDot * 10 - 5;
	look.X = upDot * 20 - 5;

	FVector rot;
	rot.Y = -25;
	
	myRight.SetLocation(myDefaultHandLocation + look + FVector(0, 0, FMath::PerlinNoise1D(myTimer * 0.5f) * 5));
	myLeft.SetLocation((myDefaultHandLocation + look) * FVector(1, -1, 1) + FVector(0, 0, FMath::PerlinNoise1D(myTimer * 0.6f) * 5));
	
	myRight.SetRotation(FQuat::MakeFromEuler(myDefaultHandRotation + rot));
	myLeft.SetRotation(FQuat::MakeFromEuler((myDefaultHandRotation + rot) * FVector(-1, 1, -1)));

	mySwordPart = 0.5f;
}

void UFPAnimator::Running(float aDT)
{
	const auto& character = GetCharacter();
	const auto& camera = GetCamera();

	// when going back from side

	const float prevSin = FMath::Sin(myTimer);	
	myTimer += aDT * mySpeed;
	const float newSin = FMath::Sin(myTimer);

	if((myStepRight && prevSin < newSin) || (!myStepRight && prevSin > newSin))
	{
		Step();
		myStepRight = !myStepRight;
	}
	
	const float off = newSin;
	myHeadRotation = off * myRotationStrength;
	myHeadPosition = FMath::Abs(off) * myPositionStrength;

	const float upDot = FMath::Clamp(FVector::DotProduct(
		character.GetActorUpVector(), camera.GetForwardVector()), 0.0f, 1.0f);

	FVector look;
	look.X = upDot * 20;
	look.Z = upDot * 5;
	look.Y = -0;
	
	FVector right;
	right.X = off * 10;
	right.Z = off * 5;
	right.Y = -off * 5;
	FVector left = right * FVector(-1, 1, -1);
	
	myRight.SetLocation(myDefaultHandLocation + right + look);
	myLeft.SetLocation((myDefaultHandLocation + look) * FVector(1, -1, 1) + left);
	
	myRight.SetRotation(FQuat::MakeFromEuler(myDefaultHandRotation));
	myLeft.SetRotation(FQuat::MakeFromEuler(myDefaultHandRotation * FVector(-1, 1, -1)));

	mySwordPart = 0.7f;
}

void UFPAnimator::WallRunning(float aDT)
{
	const auto& character = GetCharacter();
	const auto& camera = GetCamera();
	const auto& movement = GetMovement();
	const auto wallrunning = movement.GetState<UFPMovementStateWallrun>();
	CHECK_RETURN(wallrunning);
	
	myTimer += aDT * mySpeed;
	const auto wallNormal = wallrunning->GetWallNormal();
	const float rightDot = FVector::DotProduct(wallNormal, character.GetActorRightVector());
	const float forwardDot = FVector::DotProduct(wallNormal, camera.GetForwardVector());
	const float tiltOff = -rightDot * myWallrunTilt;
	const float off = FMath::Sin(myTimer);
	myHeadRotation = off * myRotationStrength + tiltOff;
	myHeadPosition = FMath::Abs(off) * myPositionStrength;

	const float upDot = FMath::Clamp(FVector::DotProduct(
		character.GetActorUpVector(), camera.GetForwardVector()), 0.0f, 1.0f);

	FVector look;
	look.Z = upDot * 10;
	look.X = upDot * 10;
	
	FVector right;
	float rightRot = 0;
	right.X = off * 5;
	right.Z = off * off * 5;
	right.Y = -10;
	FVector left = right * FVector(-1, -1, 1);
	float leftRot = 0;

	const float rTilt = FMath::Clamp(-rightDot, 0.0f, 1.0f);
	const float lTilt = FMath::Clamp(rightDot, 0.0f, 1.0f);
	//right.Y += rTilt * 20;
	right.Z += rTilt * 35;
	right.X += rTilt * 20;
	right.Y += (forwardDot * 30 + 10) * rTilt;
	rightRot -= rTilt * 120 + lTilt * 20;

	//left.Y -= lTilt * 20;
	left.Z += lTilt * 35;
	left.X += lTilt * 20;
	left.Y -= (forwardDot * 30 + 10) * lTilt;
	leftRot += lTilt * 120 + rTilt * 20;
	
	myRight.SetLocation(myDefaultHandLocation + right + look);
	myLeft.SetLocation((myDefaultHandLocation + look) * FVector(1, -1, 1) + left);

	myRight.SetRotation(FQuat::MakeFromEuler(myDefaultHandRotation +
		FVector(rightRot, 0, -20)));
	myLeft.SetRotation(FQuat::MakeFromEuler(myDefaultHandRotation *
		FVector(-1, 1, -1) + FVector(leftRot, 0, 20)));
	
	mySwordPart = 0.8f;
}

void UFPAnimator::WallClimbing(float aDT)
{
	myTimer += aDT * mySpeed * 1.2f;
	myHeadPosition = 0;
	myHeadRotation = 0;
	
	const float off = FMath::Sin(myTimer);
	const float push = FMath::Cos(myTimer);

	FVector right;
	right.X += 10 * push;
	right.Z = off * 30 + 20;

	FVector left;
	left.X += 10 * push;
	left.Z = -off * 30 + 20;

	FVector rot;
	rot.X = -45;
	rot.Y = 80 + 30 * off;
	
	myRight.SetLocation(myDefaultHandLocation + right);
	myLeft.SetLocation(myDefaultHandLocation * FVector(1, -1, 1) + left);

	myRight.SetRotation(FQuat::MakeFromEuler(myDefaultHandRotation + rot));
	myLeft.SetRotation(FQuat::MakeFromEuler((myDefaultHandRotation + rot) * FVector(-1, 1, -1)));
	mySwordPart = 0.2f;
}

void UFPAnimator::Falling(float aDT)
{
	const auto& character = GetCharacter();
	const auto& camera = GetCamera();
		
	myHeadPosition = 0;
	myHeadRotation = 0;

	const float upDot = FMath::Clamp(FVector::DotProduct(
		character.GetActorUpVector(), camera.GetForwardVector()), 0.0f, 1.0f);
	
	FVector look;
	look.Z = upDot * 10;
	look.X = upDot * 10;
	
	FVector right;
	FVector left;
	static float timer = 0;
	timer += aDT;
	float vel = character.GetVelocity().Z * 0.02f;
	right.Z = FMath::PerlinNoise1D(timer) * 5 + vel + 5;
	left.Z = FMath::PerlinNoise1D(timer * 1.3f) * 5 + vel * 0.02f + 5;
	right.Y = -vel * 0.5f;
	left.Y = vel * 0.5f;
	
	FVector rot;
	rot.X = -20;
	
	myRight.SetLocation(myDefaultHandLocation + right + look);
	myLeft.SetLocation((myDefaultHandLocation + look) * FVector(1, -1, 1) + left);

	myRight.SetRotation(FQuat::MakeFromEuler(myDefaultHandRotation + rot));
	myLeft.SetRotation(FQuat::MakeFromEuler((myDefaultHandRotation + rot) * FVector(-1, 1, -1)));

	mySwordPart = 0.2f;
}

void UFPAnimator::Interact(float aDT)
{
	CHECK_RETURN(myState == State::WALL_RUNNING);

	auto& character = GetCharacter();
	
	if (myState == State::WALL_CLIMBING)
	{
		FHitResult result;
		const FVector start = character.GetActorLocation() + FVector(0, 0, 75);
		const FVector end = start + character.GetActorForwardVector().GetSafeNormal() * 100;  
		if (!Raycast(result, start, end))
		{
			// Raycast straight down if climbing
			InteractHand(myRight, FVector(30, 0,75), FVector(30, 0, -100), FVector(-90, 90, 0));
			InteractHand(myLeft, FVector(30, 0,75), FVector(30, 0, -100), FVector(90, 90, 0));
		}
		return;		
	}

	TArray<AActor*> actors;
	GetCapsule().GetOverlappingActors(actors, AInteraction::StaticClass());
	for (const auto& it : actors)
	{
		CHECK_CONTINUE(!it);
		
		const auto interactionTrans = it->GetActorTransform();
		const auto actorTrans = GetActorTransform();
		const auto diff = interactionTrans.GetLocation() - actorTrans.GetLocation();

		// Only if point is in front of player
		const auto forward = character.GetActorForwardVector();
		const auto forwardDot = FVector::DotProduct(forward, diff.GetSafeNormal());
		CHECK_CONTINUE(forwardDot < 0.2);

		// Right or left hand?
		const auto right = character.GetActorRightVector();
		const auto rightDot = FVector::DotProduct(right, diff.GetSafeNormal());
		auto& handTrans = (rightDot > 0) ? myRight : myLeft;
		handTrans.SetLocation(actorTrans.InverseTransformPosition(interactionTrans.GetLocation()));
		handTrans.SetRotation(actorTrans.InverseTransformRotation(interactionTrans.GetRotation()));
		LOG("Touching interaction");
		return;
	}
	
	if (!InteractHand(myRight, FVector(0, 0, 0), FVector(50, 30, 40), FVector(-90, 90, 0)))
		InteractHand(myRight, FVector(0, 0, 0), FVector(50, 30, -30), FVector(-90, 90, 0));
	
	if (!InteractHand(myLeft, FVector(0, 0, 0), FVector(50, -30, 40), FVector(90, 90, 0)))
		InteractHand(myLeft, FVector(0, 0, 0), FVector(50, -30, -30), FVector(90, 90, 0));

}

bool UFPAnimator::InteractHand(FTransform& aHandTrans, const FVector& aStart, const FVector& anEnd, const FVector& aHandRotation) const
{
	auto& character = GetCharacter();
	auto actorTrans = GetActorTransform();
	
	const auto handPos = aHandTrans.GetLocation();
	const FVector start = actorTrans.TransformPosition(handPos + aStart);
	const FVector end = actorTrans.TransformPosition(handPos + anEnd);

	FHitResult result;
	if (Raycast(result, start, end))
	{
		aHandTrans.SetLocation(actorTrans.InverseTransformPosition(result.Location));
		auto rotation = actorTrans.InverseTransformRotation((result.Normal * -1.0f).Rotation().Quaternion());
		aHandTrans.SetRotation(FQuat::MakeFromEuler(rotation.Euler() + aHandRotation));
		return true;
	}
	return false;
}

bool UFPAnimator::Raycast(FHitResult& aResult, const FVector& aStart, const FVector& anEnd) const
{
	const auto& character = GetCharacter();
	//DrawDebugLine(GetWorld(), aStart, anEnd, FColor(255, 0, 0, 255), false, 0.0f, 0, 1.0f);

	constexpr ECollisionChannel channel = ECC_WorldStatic;
	FCollisionQueryParams params;
	params.bFindInitialOverlaps = false;
	params.AddIgnoredActor(&character);
	params.AddIgnoredActor(character.GetRightHand());
	params.AddIgnoredActor(character.GetLeftHand());
	
	return GetWorld()->LineTraceSingleByChannel(aResult, aStart, anEnd, channel, params);
}

void UFPAnimator::Jump(float aDT)
{
	myJumpTime += aDT;
}

void UFPAnimator::Dash(float aDT)
{
	myDashTime += aDT;
}

void UFPAnimator::Grapple(float aDT)
{
	myGrappleTime += aDT;

	if(myGrappleTime < 0.5f)
	{
		const float time = FMath::Clamp(myGrappleTime * 4.0f, 0.0f, 1.0f);
		const float part = 1 - time * time * time;
		
		const float fadeTime = FMath::Clamp((myGrappleTime - 1) * 4.0f, 0.0f, 1.0f);
		const float fadePart = FMath::Clamp(fadeTime * fadeTime, 0.0f, 1.0f);
		
		FVector left = myDefaultHandLocation * FVector(1, -1, 1);
		left.Z += 40 * part;
		left.X += 30 * part;
		left = FMath::LerpStable(left, myLeft.GetLocation(), fadePart);
		myLeft.SetLocation(left);
		myRealLeft.SetLocation(
			FMath::VInterpTo(
				myRealLeft.GetLocation(),
				myLeft.GetLocation(),
				aDT, myHandSmoothing * 2.0f));
		
		FVector rot = myDefaultHandRotation * FVector(-1, 1, -1);
		rot.X -= 90;
		rot.Y += 20;
		
		myLeft.SetRotation(FMath::Lerp(FQuat::MakeFromEuler(rot), myLeft.GetRotation(), fadePart));
	}
}

void UFPAnimator::Step() const
{
	if (myFootstepBP)
	{
		FActorSpawnParameters params;
		if (AActor* actor = GetWorld()->SpawnActor<AActor>(myFootstepBP, params))
		{
			const auto& character = GetCharacter();
			const auto& movement = GetCharacterMovement();
			actor->SetActorRotation(character.GetActorRotation());
			actor->SetActorLocation(movement.GetActorFeetLocation());
			if (myStepRight)
				actor->SetActorScale3D(FVector(1, -1, 1));
		}
	}
}
