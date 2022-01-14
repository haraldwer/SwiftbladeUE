#include "EnemyBehaviour.h"

#include "Project/Enemies/EnemyManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Project/ObjectAnimator.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Utility/MainSingelton.h"
#include "States/EnemyStateIdle.h"

UEnemyBehaviour::UEnemyBehaviour()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyBehaviour::BeginPlay()
{
	Super::BeginPlay();
	SetState(UEnemyStateIdle::StaticClass());
}

void UEnemyBehaviour::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!myCurrentState)
		SetState(UEnemyStateIdle::StaticClass());
	
	if (myCurrentState)
	{
		myCurrentState->Update(DeltaTime);
		UpdateAnimations(myCurrentState, DeltaTime);
	}	
}

void UEnemyBehaviour::SetState(const TSubclassOf<UEnemyBaseState>& aClass)
{
	UEnemyBaseState* state = Cast<UEnemyBaseState>(GetOwner()->GetComponentByClass(aClass));
	CHECK_RETURN_LOG(!state, "Invalid state");
	CHECK_RETURN_LOG(myCurrentState == state, "Already in state");
	
	myCurrentState = state;
	
	auto& animation = myCurrentState->GetAnimation();
	animation.Reset();

	myOnBehaviourStateChanged.Broadcast(myCurrentState);
}

bool UEnemyBehaviour::CanDamageTarget() const
{
	if (!myCurrentTarget.IsValid())
		return false;
	const auto self = Cast<AEnemy>(GetOwner());
	CHECK_RETURN_LOG(!self, "Invalid self", false);
	return self->IsActorInDamageHitbox(myCurrentTarget.Get());
}

AActor* UEnemyBehaviour::FindTarget()
{
	const auto owner = GetOwner();
	CHECK_RETURN_LOG(!owner, "Invalid owner", nullptr);
	const FVector location = owner->GetActorLocation();
	const FVector forward = owner->GetActorForwardVector();

	AFPCharacter* player = UMainSingelton::GetLocalPlayer();
	if (player)
	{
		if (CanTarget(player, location, forward))
			return player;
	}
	
	if (myCanTargetEnemies)
	{
		for (const auto& enemy : UMainSingelton::GetEnemyManager().GetEnemies())
		{
			if (CanTarget(enemy, location, forward))
				return enemy;
		}
	}
	
	return nullptr;
}

bool UEnemyBehaviour::CheckTargetVisibility()
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
	const FVector forward = owner->GetActorForwardVector();
	const auto pos = myCurrentTarget->GetActorLocation();
	const auto diff = pos - location;
	const auto dist = diff.Size();
	if (dist > myAttackDistance)
		return false;
	const auto dot = FVector::DotProduct(diff.GetSafeNormal(), forward);
	if (dot < myLookDot)
		return false;
	// TODO: Trace? 
	return true;
}

void UEnemyBehaviour::MoveTowards(AActor* aTarget, const float aMovementSpeed, const float aForwardWeight, const float aDT) const
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
	self->SetActorLocation(self->GetActorLocation() + dir * aMovementSpeed * aDT);
}

void UEnemyBehaviour::RotateTowards(AActor* aTarget, const float aRotationSpeed, const float aDT) const
{
	CHECK_RETURN_LOG(!aTarget, "Target nullptr");
	
	const auto self = Cast<AEnemy>(GetOwner());
	CHECK_RETURN_LOG(!self, "Unable to get self");
	
	const auto target = aTarget;
	const auto targetLocation = target->GetActorLocation();
	
	const auto rotation = self->GetActorRotation();
	const auto location = self->GetActorLocation();
	
	const auto desiredRotation = UKismetMathLibrary::FindLookAtRotation(location, targetLocation);
	const auto resultingRotation = UKismetMathLibrary::RInterpTo(rotation, desiredRotation, aDT, aRotationSpeed);
	self->SetActorRotation(resultingRotation);
}

bool UEnemyBehaviour::CanTarget(const TWeakObjectPtr<AActor>& anActor, const FVector& aLocation, const FVector& aForward)
{
	CHECK_RETURN_LOG(!anActor.IsValid(), "Invalid actor", false);
	const auto pos = anActor->GetActorLocation();
	const auto diff = pos - aLocation;
	const auto dist = diff.Size();
	if (dist < myHearDistance)
		return true;
	const auto dot = FVector::DotProduct(diff.GetSafeNormal(), aForward);
	if (dot < myLookDot)
		return false;
	const auto validDist = myCurrentTarget == anActor ? myLookDistance : myTargetDistance;
	if (dist >= validDist)
		return false;
	return true;
}


void UEnemyBehaviour::UpdateAnimations(UEnemyBaseState* aState, float aDT)
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
