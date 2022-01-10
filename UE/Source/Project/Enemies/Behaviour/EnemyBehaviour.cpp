#include "EnemyBehaviour.h"

#include "Project/Enemies/EnemyManager.h"
#include "Kismet/KismetMathLibrary.h"
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

	if (myCurrentState)
		myCurrentState->Update(DeltaTime);
	else
		SetState(UEnemyStateIdle::StaticClass());
}

void UEnemyBehaviour::SetState(const TSubclassOf<UEnemyBaseState>& aClass)
{
	UEnemyBaseState* state = Cast<UEnemyBaseState>(GetOwner()->GetComponentByClass(aClass));
	CHECK_RETURN_LOG(!state, "Invalid state");
	myCurrentState = state;
}

bool UEnemyBehaviour::CanDamageTarget() const
{
	if (!myCurrentTarget)
		return false;
	auto self = Cast<AEnemy>(GetOwner());
	CHECK_ASSERT(!self, "Invalid self");
	return self->IsActorInDamageHitbox(myCurrentTarget);
}

AActor* UEnemyBehaviour::FindTarget()
{
	const FVector location = GetOwner()->GetActorLocation();
	const FVector forward = GetOwner()->GetActorForwardVector();

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
	if (!myCurrentTarget)
		return false;
	const FVector location = GetOwner()->GetActorLocation();
	const FVector forward = GetOwner()->GetActorForwardVector();
	return CanTarget(myCurrentTarget, location, forward);
}

bool UEnemyBehaviour::CanAttackTarget() const
{
	if (!myCurrentTarget)
		return false;
	const FVector location = GetOwner()->GetActorLocation();
	const FVector forward = GetOwner()->GetActorForwardVector();
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
	const auto self = Cast<AEnemy>(GetOwner());
	CHECK_RETURN_LOG(!self, "Unable to get self");
	
	const auto target = GetTarget();
	const auto targetLocation = target->GetActorLocation();
	
	const auto location = self->GetActorLocation();
	const auto forward = self->GetActorForwardVector();
	
	const auto diff = targetLocation - location;
	const auto dir = UKismetMathLibrary::VLerp(diff.GetSafeNormal(), forward, aForwardWeight);
	self->SetActorLocation(self->GetActorLocation() + dir * aMovementSpeed * aDT);
}

void UEnemyBehaviour::RotateTowards(AActor* aTarget, const float aRotationSpeed, const float aDT) const
{
	const auto self = Cast<AEnemy>(GetOwner());
	CHECK_RETURN_LOG(!self, "Unable to get self");
	
	const auto target = GetTarget();
	const auto targetLocation = target->GetActorLocation();
	
	const auto rotation = self->GetActorRotation();
	const auto location = self->GetActorLocation();
	
	const auto desiredRotation = UKismetMathLibrary::FindLookAtRotation(location, targetLocation);
	const auto resultingRotation = UKismetMathLibrary::RInterpTo(rotation, desiredRotation, aDT, aRotationSpeed);
	self->SetActorRotation(resultingRotation);
}

bool UEnemyBehaviour::CanTarget(AActor* anActor, const FVector& aLocation, const FVector& aForward)
{
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

