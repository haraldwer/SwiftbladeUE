#include "FPCamera.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	camera.SetRelativeLocation(FVector(0, 0, height));

	// FOV
	camera.AddAdditiveOffset(myOffset, myAdditiveFov);
}

void UFPCamera::SetHeight(const float aHeight)
{
	myHeight = aHeight;
}

void UFPCamera::SetTargetHeight(const float aHeight)
{
	myTargetHeight = aHeight;
}

void UFPCamera::AddAdditiveFov(const float aFov)
{
	myAdditiveFov += aFov; 
}

void UFPCamera::SetOffset(const FTransform& anOffset)
{
	myOffset = anOffset;
}
