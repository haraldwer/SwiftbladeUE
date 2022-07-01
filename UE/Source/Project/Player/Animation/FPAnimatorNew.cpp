#include "FPAnimatorNew.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/FPCamera.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Actors/Hand.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Combat/FPCombat.h"
#include "Project/Player/Movement/FPMovement.h"
#include "Project/Utility/Tools/CustomCamera.h"
#include "States/FPAnimationStateBase.h"
#include "States/FPAnimationStateIdle.h"

void UFPAnimatorNew::BeginPlay()
{
	Super::BeginPlay();
	
	const auto& character = GetCharacter();
	AddTickPrerequisiteComponent(Cast<UActorComponent>(character.GetMovement()));
	AddTickPrerequisiteComponent(Cast<UActorComponent>(character.GetCombat()));
}

void UFPAnimatorNew::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Update states
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Interp result
	UpdateStepUp(DeltaTime);
	UpdateHands(DeltaTime);
	UpdateCamera(DeltaTime);
}

void UFPAnimatorNew::UpdateHands(const float aDT)
{
	const auto& character = GetCharacter();
	
	const auto left = character.GetLeftHand();
	CHECK_RETURN_LOG(!left, "No left");
	const auto right = character.GetRightHand();
	CHECK_RETURN_LOG(!right, "No right");
	const auto combat = character.GetCombat();
	CHECK_RETURN_LOG(!combat, "No combat");

	myRealHands = FFPAnimationHandPositions::Interp(myRealHands, myTargetHands, aDT);
	
	auto stepUpAdjusted = myRealHands;
	stepUpAdjusted.myLeft.SetLocation(stepUpAdjusted.myLeft.GetLocation() + FVector::UpVector * myCurrentStepUp);
	stepUpAdjusted.myRight.SetLocation(stepUpAdjusted.myRight.GetLocation() + FVector::UpVector * myCurrentStepUp);

	// Set result
	left->SetActorRelativeTransform(stepUpAdjusted.myLeft);
	left->SetHandState(stepUpAdjusted.myLeftHandState);
	left->SetActorRelativeScale3D(FVector(1, -1, 1));
	
	right->SetActorRelativeTransform(stepUpAdjusted.myRight);
	right->SetHandState(stepUpAdjusted.myRightHandState);

	// Move sword
	if (combat->HasSword())
		if (const auto sword = combat->GetSword())
			sword->SetActorTransform(right->GetTransform());
}

void UFPAnimatorNew::UpdateCamera(const float aDT)
{
	myRealCamera = FFPAnimationCameraData::Interp(myRealCamera, myTargetCamera, aDT);
	
	const auto& character = GetCharacter();
	const auto camera = character.GetFPCamera();
	CHECK_RETURN_LOG(!camera, "No camera");

	// Offset
	camera->SetOffset(FTransform(
		FQuat(FVector(1, 0, 0), myRealCamera.myTilt), // Rotation
			FVector(0, 0, myRealCamera.myHeight))); // Position

	// FOV
	camera->AddAdditiveFov(myRealCamera.myFOV);
}

AFPCharacter& UFPAnimatorNew::GetCharacter() const
{
	return *Cast<AFPCharacter>(GetOwner());
}

void UFPAnimatorNew::UpdateStepUp(const float aDT)
{
	const auto& character = GetCharacter();
	const auto camera = character.GetFPCamera();
	CHECK_RETURN_LOG(!camera, "No camera");

	// Interp first
	myCurrentStepUp = FMath::FInterpTo(myCurrentStepUp, 0.0f, aDT, myStepUpLerpSpeed);
	
	const bool onGround = character.GetCharacterMovement()->IsMovingOnGround();
	const float currentZ = character.GetActorLocation().Z;
	if (myWasOnGround && onGround)
	{
		// delta z?
		const float delta = currentZ - myPreviousZ;
		myCurrentStepUp = FMath::Clamp(myCurrentStepUp - delta, -myMaxStepUp, myMaxStepUp); 
	}
	myWasOnGround = onGround;
	myPreviousZ = currentZ;
	
	camera->SetStepUpOffset(myCurrentStepUp);
}

UClass* UFPAnimatorNew::GetDefaultStateType()
{
	return UFPAnimationStateIdle::StaticClass();
}

void UFPAnimatorNew::SetHands(const FFPAnimationHandPositions& someHandPositions, const bool aSnap)
{
	myTargetHands = someHandPositions;
	if (aSnap) myRealHands = someHandPositions; 
}

void UFPAnimatorNew::SetCamera(const FFPAnimationCameraData& someCameraData, const bool aSnap)
{
	myTargetCamera = someCameraData;
	if (aSnap)
		myRealCamera = someCameraData;
}


