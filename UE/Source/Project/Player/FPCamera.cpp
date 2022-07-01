#include "FPCamera.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/Movement/FPMovement.h"
#include "Project/Player/Animation/FPAnimatorNew.h"
#include "Project/Utility/Tools/CustomCamera.h"

UFPCamera::UFPCamera()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFPCamera::BeginPlay()
{
	Super::BeginPlay();

	AddTickPrerequisiteComponent(Cast<UActorComponent>(&GetMovement()));
	AddTickPrerequisiteComponent(Cast<UActorComponent>(&GetAnimator()));
	
	myOriginalHeight = GetCamera().GetRelativeLocation().Z + GetCapsule().GetScaledCapsuleHalfHeight();
}

void UFPCamera::BeginTick(float aDT)
{
	auto& camera = GetCamera();
	camera.ClearAdditiveOffset();
	myTargetHeight = 1.0f;
	myAdditiveFov = 0.0f;
}

void UFPCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto& camera = GetCamera();

	// Height
	myHeight = FMath::FInterpTo(
		myHeight,
		myTargetHeight,
		DeltaTime,
		myCameraHeightSmoothing);
	
	const float height = myHeight * myOriginalHeight - GetCapsule().GetScaledCapsuleHalfHeight();
	camera.SetRelativeLocation(FVector(0, 0, height + myStepUpOffset));

	// FOV
	camera.AddAdditiveOffset(myOffset, myAdditiveFov);
}