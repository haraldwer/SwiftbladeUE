#include "FPAnimatorNew.h"

#include "GameFramework/PawnMovementComponent.h"
#include "Project/Player/FPCamera.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/Actors/Hand.h"
#include "Project/Player/Actors/Sword.h"
#include "Project/Player/Combat/FPCombat.h"
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

	// Set result
	left->SetActorRelativeTransform(myRealHands.myLeft);
	left->SetHandState(myRealHands.myLeftHandState);
	left->SetActorRelativeScale3D(FVector(1, -1, 1));
	
	right->SetActorRelativeTransform(myRealHands.myRight);
	right->SetHandState(myRealHands.myRightHandState);

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
	camera->SetOffset(FTransform(
		FQuat(FVector(1, 0, 0), myRealCamera.myTilt), // Rotation
			FVector(0, 0, myRealCamera.myHeight))); // Position
	camera->AddAdditiveFov(myRealCamera.myFOV);	
}

AFPCharacter& UFPAnimatorNew::GetCharacter() const
{
	return *Cast<AFPCharacter>(GetOwner());
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


