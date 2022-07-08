#include "EnemyStateAttackStare.h"

#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Enemies/Behaviour/EnemyBehaviour.h"
#include "Project/Player/FPCamera.h"
#include "Project/Player/FPCharacter.h"

UEnemyStateAttackStare::UEnemyStateAttackStare()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyStateAttackStare::Charge(const float aDT)
{
	myStareValue = myStareInitialPart * myAttackTime;
	mySmoothFov = 0.0f; 
	
	// Set target initial rotation
	if (const auto target = Cast<APawn>(GetBehaviour().GetTarget()))
		myTargetPreviousRotation = target->GetControlRotation();
	
	// Skip this state
	SetSubState(EEnemyAttackState::ATTACK);
}

void UEnemyStateAttackStare::Attack(const float aDT)
{
	const auto& self = GetSelf(); 
	const auto& behaviour = GetBehaviour();
	const auto target = Cast<APawn>(behaviour.GetTarget());
	if (!target)
	{
		SetSubState(EEnemyAttackState::RECOVER);
		return;
	}

	const auto targetRot = target->GetControlRotation();
	const FVector targetEyeLoc = target->GetPawnViewLocation();
	const FVector location = self.GetActorLocation();
	const FVector diff = targetEyeLoc - location;
	
	// Check valid dot
	const float dot = self.GetActorForwardVector().GetSafeNormal().Dot(
		diff.GetSafeNormal());
	if (dot < myStareValidDot)
	{
		SetSubState(EEnemyAttackState::RECOVER);
		return;
	}
	
	// Rotate slowly
	behaviour.RotateTowards(target, myStareRotationSpeed, aDT);

	// Increase stare
	const float dist = diff.Size();
	const float distPart = FMath::Max(dist - myStareDistStart, 0.0f) / (myStareDistEnd - myStareDistStart);
	const float distMul = FMath::Lerp(1.0f, myStareDistEndMul, distPart);
	myStareValue += distMul * aDT;

	// Decrease stare
	// How much has rotation changed? 
	const float rotChangedDot = myTargetPreviousRotation.Vector().Dot(targetRot.Vector());
	const float rotChanged = 1.0f - (rotChangedDot + 1.0f) * 0.5f;
	myStareValue -= rotChanged * myStarePullMul;
	
	// Release if pull fast enough
	if (myStareValue < 0)
	{
		SetSubState(EEnemyAttackState::RECOVER);
		return;
	}

	// Kill if stare long enough
	if (myStareValue >= myAttackTime)
	{
		PerformAttack(target);
		SetSubState(EEnemyAttackState::RECOVER);
		return;
	}

	// Interp speed 
	const float starePart = (myStareValue / myAttackTime);
	const float interpSpeed = FMath::Lerp(myStareRotInterpStart, myStareRotInterpEnd, starePart);
	
	// Stare, apply on rotation
	const auto lookAtRotation =
		UKismetMathLibrary::FindLookAtRotation(
			targetEyeLoc,
			location);
	const auto result = FMath::RInterpTo(
		targetRot,
		lookAtRotation,
		aDT,
		interpSpeed);
	myTargetPreviousRotation = result; 
	if (const auto controller = target->GetController())
		controller->SetControlRotation(result);
	
	const float fov = FMath::Lerp(myStareStartFov, myStareEndFov, starePart);
	mySmoothFov = FMath::FInterpTo(mySmoothFov, fov, aDT, myFovInterpSpeed);
	if (const auto character = Cast<AFPCharacter>(target))
	{
		character->SetPPScalar(PP_EYE, "Strength", starePart);
		if (const auto camera = character->GetFPCamera())
			camera->AddAdditiveFov(mySmoothFov);
	}
}

void UEnemyStateAttackStare::Recover(const float aDT)
{
	Super::Recover(aDT);
}

void UEnemyStateAttackStare::PerformAttack(AActor* aTarget)
{
	CHECK_RETURN_LOG(!aTarget, "No target");
	auto& self = GetSelf();
	const auto controller = GetSelf().GetController();
	UGameplayStatics::ApplyDamage(aTarget, 1.0f, controller, &self, UDamageType::StaticClass());	 
	Super::PerformAttack(aTarget);
}
