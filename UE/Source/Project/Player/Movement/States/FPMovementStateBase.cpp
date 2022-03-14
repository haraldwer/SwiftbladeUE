#include "FPMovementStateBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"
#include "Project/Player/Animation/FPAnimator.h"

void UFPMovementStateBase::Jump(FVector aDirection) const
{
	auto& movement = GetCharacterMovement();
	movement.Velocity.Z = 0;
	movement.AddImpulse(aDirection * movement.JumpZVelocity, true);
	auto& animator = GetAnimator(); 
	animator.StartJump();
}

float UFPMovementStateBase::GetSensitivity() const
{
	return GetCharacter().GetSensitivity(); 
}

UClass* UFPMovementStateBase::Input(EFPMovementInputAction anAction, float aValue)
{
	switch (anAction) {
	case EFPMovementInputAction::MOVE_HORIZONTAL:	MoveHorizontal(aValue); break;
	case EFPMovementInputAction::MOVE_VERTICAL:		MoveVertical(aValue); break;
	case EFPMovementInputAction::LOOK_HORIZONTAL:	LookHorizontal(aValue); break;
	case EFPMovementInputAction::LOOK_VERTICAL:		LookVertical(aValue); break;
	}
	return nullptr;
}

void UFPMovementStateBase::MoveHorizontal(const float aValue) const
{ 
	GetCharacter().AddMovementInput(FRotationMatrix(
			GetController().GetControlRotation()).GetScaledAxis(EAxis::Y), aValue);
}

void UFPMovementStateBase::MoveVertical(const float aValue) const
{
	auto rot = FRotationMatrix(
		GetController().GetControlRotation()).GetScaledAxis(EAxis::X);
	rot.Z = 0.0f;
	rot.Normalize();
	GetCharacter().AddMovementInput(rot, aValue);
}

void UFPMovementStateBase::LookHorizontal(const float aValue) const
{
	GetCharacter().AddControllerYawInput(aValue * GetSensitivity());
}

void UFPMovementStateBase::LookVertical(const float aValue) const
{
	GetCharacter().AddControllerPitchInput(aValue * GetSensitivity() * -1.0f);
}


AFPController& UFPMovementStateBase::GetController() const
{
	const auto ptr = Cast<AFPController>(GetCharacter().GetController());
	CHECK_ASSERT(!ptr, "Controller nullptr");
	return *ptr;
}

AFPCharacter& UFPMovementStateBase::GetCharacter() const
{
	const auto ptr = Cast<AFPCharacter>(GetOwner());
	CHECK_ASSERT(!ptr, "Character nullptr");
	return *ptr;
}

FTransform UFPMovementStateBase::GetActorTransform() const
{
	return GetCharacter().GetActorTransform();
}

UCapsuleComponent& UFPMovementStateBase::GetCapsule() const
{
	const auto ptr = GetCharacter().GetCapsuleComponent();
	CHECK_ASSERT(!ptr, "Capsule nullptr");
	return *ptr;
}

UCharacterMovementComponent& UFPMovementStateBase::GetCharacterMovement() const
{
	const auto ptr = GetCharacter().GetCharacterMovement();
	CHECK_ASSERT(!ptr, "CharacterMovement nullptr");
	return *ptr;
}

UCameraComponent& UFPMovementStateBase::GetCamera() const
{
	const auto ptr = GetCharacter().GetCamera();
	CHECK_ASSERT(!ptr, "Camera nullptr");
	return *ptr;
}

UFPCamera& UFPMovementStateBase::GetFPCamera() const
{
	const auto ptr = GetCharacter().GetFPCamera();
	CHECK_ASSERT(!ptr, "FPCamera nullptr");
	return *ptr;
}

UFPAnimator& UFPMovementStateBase::GetAnimator() const
{
	const auto ptr = GetCharacter().GetAnimator();
	CHECK_ASSERT(!ptr, "Animator nullptr");
	return *ptr;
}

UFPMovementStateMachine& UFPMovementStateBase::GetMovement() const
{
	const auto ptr = GetCharacter().GetMovement();
	CHECK_ASSERT(!ptr, "Movement nullptr");
	return *ptr;
}

UFPCombat& UFPMovementStateBase::GetCombat() const
{
	const auto ptr = GetCharacter().GetCombat();
	CHECK_ASSERT(!ptr, "Combat nullptr");
	return *ptr;
}
