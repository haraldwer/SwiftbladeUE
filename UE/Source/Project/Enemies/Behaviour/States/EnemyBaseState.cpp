#include "EnemyBaseState.h"

#include "Components/StaticMeshComponent.h"
#include "Project/Enemies/Enemy.h"
#include "Project/Enemies/EnemyBehaviour.h"

UEnemyBaseState::UEnemyBaseState()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyBaseState::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemyBaseState::SetState(const TSubclassOf<UEnemyBaseState>& aStateType)
{
	UEnemyBehaviour* behaviour = Cast<UEnemyBehaviour>(GetSelf().GetComponentByClass(UEnemyBehaviour::StaticClass()));
	CHECK_RETURN_LOG(!behaviour, "Could not find enemy behaviour");
	const auto previousState = behaviour->GetState();
	behaviour->SetState(aStateType);
	const auto state = behaviour->GetState();
	if (state)
		state->Enter(previousState);
}

UEnemyBehaviour& UEnemyBaseState::GetBehaviour() const
{
	UEnemyBehaviour* behaviour = Cast<UEnemyBehaviour>(GetSelf().GetComponentByClass(UEnemyBehaviour::StaticClass()));
	CHECK_ASSERT(!behaviour, "Could not find enemy behaviour");
	return *behaviour; 
}

UStaticMeshComponent& UEnemyBaseState::GetMesh() const
{
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(GetSelf().GetComponentByClass(UStaticMeshComponent::StaticClass()));
	CHECK_ASSERT(!mesh, "Could not find mesh component");
	return *mesh;
}

AEnemy& UEnemyBaseState::GetSelf() const
{
	AEnemy* enemy = Cast<AEnemy>(GetOwner());
	CHECK_ASSERT(!enemy, "Owner is invalid")
	return *enemy;
}