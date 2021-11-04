
#include "FPComponentBase.h"
#include "Project/Utility.h"
#include "FPCharacter.h"
#include "FPController.h"

UFPComponentBase::UFPComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

AFPController& UFPComponentBase::GetController() const
{
	const auto ptr = Cast<AFPController>(GetCharacter().GetController());
	CHECK_ASSERT(!ptr)
	return *ptr;
}

AFPCharacter& UFPComponentBase::GetCharacter() const
{
	const auto ptr = Cast<AFPCharacter>(GetOwner());
	CHECK_ASSERT(!ptr);
	return *ptr;
}

FTransform UFPComponentBase::GetActorTransform() const
{
	return GetCharacter().GetActorTransform();
}

UCapsuleComponent& UFPComponentBase::GetCapsule() const
{
	const auto ptr = GetCharacter().GetCapsuleComponent();
	CHECK_ASSERT(!ptr);
	return *ptr;
}

UCharacterMovementComponent& UFPComponentBase::GetCharacterMovement() const
{
	const auto ptr = GetCharacter().GetCharacterMovement();
	CHECK_ASSERT(!ptr);
	return *ptr;
}

UCameraComponent& UFPComponentBase::GetCamera() const
{
	const auto ptr = GetCharacter().GetCamera();
	CHECK_ASSERT(!ptr);
	return *ptr;
}

UFPAnimator& UFPComponentBase::GetAnimator() const
{
	const auto ptr = GetCharacter().GetAnimator();
    CHECK_ASSERT(!ptr);
    return *ptr;
}

UFPMovement& UFPComponentBase::GetMovement() const
{
	const auto ptr = GetCharacter().GetMovement();
	CHECK_ASSERT(!ptr);
	return *ptr;
}

UFPCombat& UFPComponentBase::GetCombat() const
{
	const auto ptr = GetCharacter().GetCombat();
	CHECK_ASSERT(!ptr);
	return *ptr;
}
