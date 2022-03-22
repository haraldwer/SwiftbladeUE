
#include "FPComponentBase.h"
#include "FPCharacter.h"
#include "FPController.h"

UFPActorComponentBase::UFPActorComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

AFPController& UFPActorComponentBase::GetController() const
{
	const auto ptr = Cast<AFPController>(GetCharacter().GetController());
	CHECK_ASSERT(!ptr, "Controller nullptr");
	return *ptr;
}

AFPCharacter& UFPActorComponentBase::GetCharacter() const
{
	const auto ptr = Cast<AFPCharacter>(GetOwner());
	CHECK_ASSERT(!ptr, "Character nullptr");
	return *ptr;
}

FTransform UFPActorComponentBase::GetActorTransform() const
{
	return GetCharacter().GetActorTransform();
}

UCapsuleComponent& UFPActorComponentBase::GetCapsule() const
{
	const auto ptr = GetCharacter().GetCapsuleComponent();
	CHECK_ASSERT(!ptr, "Capsule nullptr");
	return *ptr;
}

UCharacterMovementComponent& UFPActorComponentBase::GetCharacterMovement() const
{
	const auto ptr = GetCharacter().GetCharacterMovement();
	CHECK_ASSERT(!ptr, "CharacterMovement nullptr");
	return *ptr;
}

UCameraComponent& UFPActorComponentBase::GetCamera() const
{
	const auto ptr = GetCharacter().GetCamera();
	CHECK_ASSERT(!ptr, "Camera nullptr");
	return *ptr;
}

UFPCamera& UFPActorComponentBase::GetFPCamera() const
{
	const auto ptr = GetCharacter().GetFPCamera();
	CHECK_ASSERT(!ptr, "Camera nullptr");
	return *ptr;
}

UFPAnimator& UFPActorComponentBase::GetAnimator() const
{
	const auto ptr = GetCharacter().GetAnimator();
    CHECK_ASSERT(!ptr, "Animator nullptr");
    return *ptr;
}

UFPMovement& UFPActorComponentBase::GetMovement() const
{
	const auto ptr = GetCharacter().GetMovement();
	CHECK_ASSERT(!ptr, "Movement nullptr");
	return *ptr;
}

UFPCombat& UFPActorComponentBase::GetCombat() const
{
	const auto ptr = GetCharacter().GetCombat();
	CHECK_ASSERT(!ptr, "Combat nullptr");
	return *ptr;
}

AEffect* UFPActorComponentBase::CreateEffect(const TSubclassOf<AEffect>& aBP, const FTransform& aTransform) const
{
	return GetCharacter().CreateEffect(aBP, aTransform);
}

UFPSceneComponentBase::UFPSceneComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

AFPController& UFPSceneComponentBase::GetController() const
{
	const auto ptr = Cast<AFPController>(GetCharacter().GetController());
	CHECK_ASSERT(!ptr, "Controller nullptr");
	return *ptr;
}

AFPCharacter& UFPSceneComponentBase::GetCharacter() const
{
	const auto ptr = Cast<AFPCharacter>(GetOwner());
	CHECK_ASSERT(!ptr, "Character nullptr");
	return *ptr;
}

FTransform UFPSceneComponentBase::GetActorTransform() const
{
	return GetCharacter().GetActorTransform();
}

UCapsuleComponent& UFPSceneComponentBase::GetCapsule() const
{
	const auto ptr = GetCharacter().GetCapsuleComponent();
	CHECK_ASSERT(!ptr, "Capsule nullptr");
	return *ptr;
}

UCharacterMovementComponent& UFPSceneComponentBase::GetCharacterMovement() const
{
	const auto ptr = GetCharacter().GetCharacterMovement();
	CHECK_ASSERT(!ptr, "CharacterMovement nullptr");
	return *ptr;
}

UCameraComponent& UFPSceneComponentBase::GetCamera() const
{
	const auto ptr = GetCharacter().GetCamera();
	CHECK_ASSERT(!ptr, "Camera nullptr");
	return *ptr;
}

UFPCamera& UFPSceneComponentBase::GetFPCamera() const
{
	const auto ptr = GetCharacter().GetFPCamera();
	CHECK_ASSERT(!ptr, "Camera nullptr");
	return *ptr;
}

UFPAnimator& UFPSceneComponentBase::GetAnimator() const
{
	const auto ptr = GetCharacter().GetAnimator();
	CHECK_ASSERT(!ptr, "Animator nullptr");
	return *ptr;
}

UFPMovement& UFPSceneComponentBase::GetMovement() const
{
	const auto ptr = GetCharacter().GetMovement();
	CHECK_ASSERT(!ptr, "Movement nullptr");
	return *ptr;
}

UFPCombat& UFPSceneComponentBase::GetCombat() const
{
	const auto ptr = GetCharacter().GetCombat();
	CHECK_ASSERT(!ptr, "Combat nullptr");
	return *ptr;
}

AEffect* UFPSceneComponentBase::CreateEffect(const TSubclassOf<AEffect>& aBP, const FTransform& aTransform) const
{
	return GetCharacter().CreateEffect(aBP, aTransform);
}
