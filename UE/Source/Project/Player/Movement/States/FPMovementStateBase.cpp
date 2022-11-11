#include "FPMovementStateBase.h"

#include "FPMovementStateDash.h"
#include "FPMovementStateInAir.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Player/FPController.h"
#include "Project/Player/Combat/FPCombat.h"
#include "Project/Player/Actors/Hand.h"
#include "Project/Player/Actors/Sword.h"

void UFPMovementStateBase::ResetAbilities()
{
	if (const auto dashState = GetState<UFPMovementStateDash>())
		dashState->Reset();
	if (const auto airState = GetState<UFPMovementStateInAir>())
		airState->ResetJumps();
}

void UFPMovementStateBase::Jump(const FVector aDirection) const
{
	auto& movement = GetCharacterMovement();
	movement.Velocity.Z = 0;
	movement.AddImpulse(aDirection * movement.JumpZVelocity, true);
}

float UFPMovementStateBase::GetSensitivity() const
{
	return GetCharacter().GetSensitivity(); 
}

UClass* UFPMovementStateBase::Input(const EFPMovementInputAction anAction, const float aValue)
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
	const float dilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	GetCharacter().AddControllerYawInput(aValue * GetSensitivity() * dilation);
}

void UFPMovementStateBase::LookVertical(const float aValue) const
{
	const float dilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	GetCharacter().AddControllerPitchInput(aValue * GetSensitivity() * -1.0f * dilation);
}

bool UFPMovementStateBase::HasMagic() const
{
	return GetCharacter().HasMagic();
}

TArray<FHitResult> UFPMovementStateBase::MultiSweep(const FVector& aStart, const FVector& aEnd, const float aRadius) const
{
	TArray<FHitResult> hits;
	FCollisionQueryParams params;
	params.bFindInitialOverlaps = true;
	params.AddIgnoredActor(Cast<AActor>(GetCharacter().GetLeftHand()));
	params.AddIgnoredActor(Cast<AActor>(GetCharacter().GetRightHand()));
	params.AddIgnoredActor(&GetCharacter());
	if (const auto combat = GetCharacter().GetCombat())
		params.AddIgnoredActor(Cast<AActor>(combat->GetSword()));

	if (aRadius > KINDA_SMALL_NUMBER)
		GetWorld()->SweepMultiByChannel(hits, aStart, aEnd,
			FQuat::Identity, ECC_WorldStatic,
			FCollisionShape::MakeSphere(aRadius), params);
	else
		GetWorld()->LineTraceMultiByChannel(hits, aStart, aEnd,
			ECC_WorldStatic, params);
	
	return hits;
}

FHitResult UFPMovementStateBase::Sweep(const FVector& aStart, const FVector& aEnd, const float aRadius) const
{
	FHitResult hit;
	FCollisionQueryParams params;
	params.bFindInitialOverlaps = true;
	params.AddIgnoredActor(Cast<AActor>(GetCharacter().GetLeftHand()));
	params.AddIgnoredActor(Cast<AActor>(GetCharacter().GetRightHand()));
	params.AddIgnoredActor(&GetCharacter());
	if (const auto combat = GetCharacter().GetCombat())
		params.AddIgnoredActor(Cast<AActor>(combat->GetSword()));

	if (aRadius > KINDA_SMALL_NUMBER)
		GetWorld()->SweepSingleByChannel(hit, aStart, aEnd,
			FQuat::Identity, ECC_WorldStatic,
			FCollisionShape::MakeSphere(aRadius), params);
	else
		GetWorld()->LineTraceSingleByChannel(hit, aStart, aEnd,
			ECC_WorldStatic, params);
	
	return hit;
}
