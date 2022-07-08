#include "EnemyBehaviour.h"

#include "GameFramework/PawnMovementComponent.h"
#include "Project/Enemies/EnemyManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Project/Player/FPCharacter.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Utility/Tools/Animation/ObjectAnimator.h"
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

AActor* UEnemyBehaviour::FindTarget() const
{
	const auto owner = GetOwner();
	CHECK_RETURN_LOG(!owner, "Invalid owner", nullptr);
	const FVector location = owner->GetActorLocation();
	const FVector forward = owner->GetActorForwardVector();

	if (AFPCharacter* player = UMainSingelton::GetLocalPlayer())
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

	if (const auto collider = self->GetCollider())
	{
		collider->AddForce(dir * aMovementSpeed, NAME_None, true);
		DrawDebugLine(GetWorld(),
			collider->GetComponentLocation(),
			collider->GetComponentLocation() + collider->GetComponentVelocity(),
			FColor(0, 255, 0, 255));
	}
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

	const auto cross = rotation.Vector().Cross(desiredRotation.Vector());

	// adjust roll so that right is correct
	const auto updot = self->GetActorUpVector().Dot(FVector::UpVector);
	const auto rightCross = self->GetActorRightVector().Cross((self->GetActorRightVector() * updot).GetSafeNormal2D());
	
	if (const auto collider = self->GetCollider())
	{
		collider->AddTorqueInRadians(cross * aRotationSpeed, NAME_None, true);
		collider->AddTorqueInRadians(rightCross * 20.0f, NAME_None, true);
		DrawDebugLine(GetWorld(),
			collider->GetComponentLocation(),
			collider->GetComponentLocation() + rightCross * 100.0f,
			FColor(0, 0, 255, 255));
	}
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


void UEnemyBehaviour::UpdateAnimations(UEnemyBaseState* aState, float aDT) const
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
