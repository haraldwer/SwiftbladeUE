#include "EnemyAnimator.h"

UEnemyAnimator::UEnemyAnimator()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyAnimator::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemyAnimator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

