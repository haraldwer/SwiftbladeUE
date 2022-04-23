#include "FPStateBase.h"
#include "FPCharacter.h"
#include "FPController.h"

AFPController& UFPStateBase::GetController() const
{
	const auto ptr = Cast<AFPController>(GetCharacter().GetController());
	CHECK_ASSERT(!ptr, "Controller nullptr");
	return *ptr;
}

AFPCharacter& UFPStateBase::GetCharacter() const
{
	const auto ptr = Cast<AFPCharacter>(GetOwner());
	CHECK_ASSERT(!ptr, "Character nullptr");
	return *ptr;
}

FTransform UFPStateBase::GetActorTransform() const
{
	return GetCharacter().GetActorTransform();
}

UCapsuleComponent& UFPStateBase::GetCapsule() const
{
	const auto ptr = GetCharacter().GetCapsuleComponent();
	CHECK_ASSERT(!ptr, "Capsule nullptr");
	return *ptr;
}

UCharacterMovementComponent& UFPStateBase::GetCharacterMovement() const
{
	const auto ptr = GetCharacter().GetCharacterMovement();
	CHECK_ASSERT(!ptr, "CharacterMovement nullptr");
	return *ptr;
}

UCustomCamera& UFPStateBase::GetCamera() const
{
	const auto ptr = GetCharacter().GetCamera();
	CHECK_ASSERT(!ptr, "Camera nullptr");
	return *ptr;
}

UFPCamera& UFPStateBase::GetFPCamera() const
{
	const auto ptr = GetCharacter().GetFPCamera();
	CHECK_ASSERT(!ptr, "FPCamera nullptr");
	return *ptr;
}

UFPAnimatorNew& UFPStateBase::GetAnimator() const
{
	const auto ptr = GetCharacter().GetAnimator();
	CHECK_ASSERT(!ptr, "Animator nullptr");
	return *ptr;
}

UFPMovement& UFPStateBase::GetMovement() const
{
	const auto ptr = GetCharacter().GetMovement();
	CHECK_ASSERT(!ptr, "Movement nullptr");
	return *ptr;
}

UFPCombat& UFPStateBase::GetCombat() const
{
	const auto ptr = GetCharacter().GetCombat();
	CHECK_ASSERT(!ptr, "Combat nullptr");
	return *ptr;
}
