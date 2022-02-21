
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
	CHECK_ASSERT(!ptr, "Controller nullptr")
	return *ptr;
}

AFPCharacter& UFPComponentBase::GetCharacter() const
{
	const auto ptr = Cast<AFPCharacter>(GetOwner());
	CHECK_ASSERT(!ptr, "Character nullptr");
	return *ptr;
}

FTransform UFPComponentBase::GetActorTransform() const
{
	return GetCharacter().GetActorTransform();
}

UCapsuleComponent& UFPComponentBase::GetCapsule() const
{
	const auto ptr = GetCharacter().GetCapsuleComponent();
	CHECK_ASSERT(!ptr, "Capsule nullptr");
	return *ptr;
}

UCharacterMovementComponent& UFPComponentBase::GetCharacterMovement() const
{
	const auto ptr = GetCharacter().GetCharacterMovement();
	CHECK_ASSERT(!ptr, "CharacterMovement nullptr");
	return *ptr;
}

UCameraComponent& UFPComponentBase::GetCamera() const
{
	const auto ptr = GetCharacter().GetCamera();
	CHECK_ASSERT(!ptr, "Camera nullptr");
	return *ptr;
}

UFPAnimator& UFPComponentBase::GetAnimator() const
{
	const auto ptr = GetCharacter().GetAnimator();
    CHECK_ASSERT(!ptr, "Animator nullptr");
    return *ptr;
}

UFPMovement& UFPComponentBase::GetMovement() const
{
	const auto ptr = GetCharacter().GetMovement();
	CHECK_ASSERT(!ptr, "Movement nullptr");
	return *ptr;
}

UFPCombat& UFPComponentBase::GetCombat() const
{
	const auto ptr = GetCharacter().GetCombat();
	CHECK_ASSERT(!ptr, "Combat nullptr");
	return *ptr;
}

UFPMagic& UFPComponentBase::GetMagic() const
{
	const auto ptr = GetCharacter().GetMagic();
	CHECK_ASSERT(!ptr, "Magic nullptr");
	return *ptr;
}

AEffect* UFPComponentBase::CreateEffect(const TSubclassOf<AEffect>& aBP, const FTransform& aTransform) const
{
	return GetCharacter().CreateEffect(aBP, aTransform);
}
