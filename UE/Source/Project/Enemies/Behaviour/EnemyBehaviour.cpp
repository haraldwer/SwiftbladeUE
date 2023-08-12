#include "EnemyBehaviour.h"

#include "DrawDebugHelpers.h"
#include "..\EnemySubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Utility/GameUtility.h"
#include "Project/Utility/Tools/Animation/ObjectAnimator.h"
#include "States/EnemyStateIdle.h"

UEnemyBehaviour::UEnemyBehaviour()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyBehaviour::BeginPlay()
{
	Super::BeginPlay();

	if (const auto owner = GetOwner())
		for (auto& comp : owner->GetComponents())
			if (const auto state = Cast<UEnemyBaseState>(comp))
				myStates.Add(state);
	
	SetState(UEnemyStateIdle::StaticClass());
}

void UEnemyBehaviour::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (IsBeingDestroyed())
		return;
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!myCurrentState.IsValid())
		SetState(UEnemyStateIdle::StaticClass());
	
	if (const auto state = myCurrentState.Get())
	{
		state->Update(DeltaTime);
		UpdateAnimations(state, DeltaTime);
	}	
}

void UEnemyBehaviour::SetState(const TSubclassOf<UEnemyBaseState>& aClass)
{
	const auto find = myStates.FindByPredicate([aClass](const TWeakObjectPtr<UEnemyBaseState>& weakPtr)
	{
		if (const auto ptr = weakPtr.Get())
			return ptr->IsA(aClass);
		return false;
	});
	CHECK_RETURN_LOG(!find, "Could not find state");
	
	const auto state = find->Get();
	const auto currState = myCurrentState.Get(); 
	CHECK_RETURN_LOG(!state, "Invalid state");
	CHECK_RETURN_LOG(currState == state, "Already in state");
	
	myCurrentState = state;
	
	auto& animation = state->GetAnimation();
	animation.Reset();

	myOnBehaviourStateChanged.Broadcast(state);
}

bool UEnemyBehaviour::CanDamageTarget() const
{
	if (!myCurrentTarget.IsValid())
		return false;
	const auto self = Cast<AEnemy>(GetOwner());
	CHECK_RETURN_LOG(!self, "Invalid self", false);
	return self->IsActorInDamageHitbox(myCurrentTarget.Get());
}

AActor* UEnemyBehaviour::FindTarget() const
{
	const auto owner = GetOwner();
	CHECK_RETURN_LOG(!owner, "Invalid owner", nullptr);
	const FVector location = owner->GetActorLocation();
	const FVector forward = owner->GetActorForwardVector();

	if (AFPCharacter* player = UGameUtility::GetLocalPlayer())
		if (CanTarget(player, location, forward))
			return player;
	
	if (myCanTargetEnemies)
		for (const auto& enemy : UEnemySubsystem::Get().GetEnemies())
			if (const auto ptr = enemy.Get())
				if (CanTarget(ptr, location, forward))
					return ptr;
	
	return nullptr;
}

bool UEnemyBehaviour::CheckTargetVisibility() const
{
	if (!myCurrentTarget.IsValid())
		return false;
	const auto owner = GetOwner();
	CHECK_RETURN_LOG(!owner, "Invalid owner", false);
	const FVector location = owner->GetActorLocation();
	const FVector forward = owner->GetActorForwardVector();
	return CanTarget(myCurrentTarget, location, forward);
}

bool UEnemyBehaviour::CanAttackTarget() const
{
	if (!myCurrentTarget.IsValid())
		return false;
	const auto owner = GetOwner();
	CHECK_RETURN_LOG(!owner, "Invalid owner", false);
	const FVector location = owner->GetActorLocation();
	const auto pos = myCurrentTarget->GetActorLocation();
	return (pos - location).Size() <= myAttackDistance;
}

void UEnemyBehaviour::MoveTowards(const AActor* aTarget, const float aMovementSpeed, const float aForwardWeight, const float aDT) const
{
	CHECK_RETURN_LOG(!aTarget, "Target nullptr");
	
	const auto self = Cast<AEnemy>(GetOwner());
	CHECK_RETURN_LOG(!self, "Unable to get self");
	
	const auto target = aTarget;
	const auto targetLocation = target->GetActorLocation();
	
	const auto location = self->GetActorLocation();
	const auto forward = self->GetActorForwardVector();
	
	const auto diff = targetLocation - location;
	const auto dir = UKismetMathLibrary::VLerp(diff.GetSafeNormal(), forward, aForwardWeight);

	if (const auto collider = self->GetCollider())
	{
		collider->AddForce(dir * aMovementSpeed, NAME_None, true);
		if (myDrawDebugForces)
			DrawDebugLine(GetWorld(),
				collider->GetComponentLocation(),
				collider->GetComponentLocation() + collider->GetComponentVelocity(),
				FColor(0, 255, 0, 255));
	}
}

void UEnemyBehaviour::RotateTowards(const AActor* aTarget, const float aRotationSpeed, const float aDT) const
{
	CHECK_RETURN_LOG(!aTarget, "Target nullptr");
	
	const auto self = Cast<AEnemy>(GetOwner());
	CHECK_RETURN_LOG(!self, "Unable to get self");
	
	const auto target = aTarget;
	const auto targetLocation = target->GetActorLocation();
	
	const auto rotation = self->GetActorRotation();
	const auto location = self->GetActorLocation();
	
	const auto desiredRotation = UKismetMathLibrary::FindLookAtRotation(location, targetLocation);

	const auto cross = rotation.Vector().Cross(desiredRotation.Vector());

	// adjust roll so that right is correct
	const auto updot = self->GetActorUpVector().Dot(FVector::UpVector);
	const auto rightCross = self->GetActorRightVector().Cross((self->GetActorRightVector() * updot).GetSafeNormal2D());
	
	if (const auto collider = self->GetCollider())
	{
		collider->AddTorqueInRadians(cross * aRotationSpeed, NAME_None, true);
		collider->AddTorqueInRadians(rightCross * 20.0f, NAME_None, true);
		if (myDrawDebugForces)
			DrawDebugLine(GetWorld(),
				collider->GetComponentLocation(),
				collider->GetComponentLocation() + rightCross * 100.0f,
				FColor(0, 0, 255, 255));
	}
}

void UEnemyBehaviour::OnTookDamage(const float aDamageAmount, AActor* aDamageCauser)
{
	for (auto& state : myStates)
		if (const auto ptr = state.Get())
			ptr->OnTookDamage(aDamageAmount, aDamageCauser);
}

void UEnemyBehaviour::OnDied()
{
	for (auto& state : myStates)
		if (const auto ptr = state.Get())
			ptr->OnDied();
}

bool UEnemyBehaviour::CanTarget(const TWeakObjectPtr<AActor>& anActor, const FVector& aLocation, const FVector& aForward) const
{
	auto actor = anActor.Get();
	CHECK_RETURN_LOG(!actor, "Invalid actor", false);
	const auto pos = actor->GetActorLocation();
	const auto diff = pos - aLocation;
	const auto dist = diff.Size();

	// In the right direction? 
	const auto dot = FVector::DotProduct(diff.GetSafeNormal(), aForward);
	if (dot < myLookDot)
	{
		// Hear distance
		if (dist >= myHearDistance)
			return false;
	}

	// Within distance? 
	const auto validDist = myCurrentTarget == actor ? myLookDistance : myTargetDistance;
	if (dist >= validDist)
		return false;

	FHitResult result;
	FCollisionQueryParams params;
	params.AddIgnoredActor(actor);
	params.AddIgnoredActor(GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(result, aLocation, pos, ECC_WorldStatic, params))
		return false;
	
	return true;
}


void UEnemyBehaviour::UpdateAnimations(UEnemyBaseState* aState, const float aDT) const
{
	CHECK_RETURN_LOG(!aState, "Invalid state");
	
	auto& animation = aState->GetAnimation();
	
	TArray<FObjectAnimatorKey> someKeys;
	FString anEventName;
	if (animation.Update(aDT, someKeys, anEventName))
	{
		const auto self = Cast<AEnemy>(GetOwner());
		CHECK_RETURN_LOG(!self, "Invalid self");
		const auto animator = self->GetObjectAnimator();
		CHECK_RETURN_LOG(!animator, "Invalid object animator");
		animator->SetKeys(someKeys);
		if (!anEventName.IsEmpty())
			myOnAnimationEvent.Broadcast(anEventName);
	}
}
