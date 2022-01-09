#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyBaseState.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyBaseState : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyBaseState();
	virtual void BeginPlay() override;
	virtual void Enter(const UEnemyBaseState* aFromState) {}
	virtual void Update(float aDT) {}

protected:
	void SetState(const TSubclassOf<UEnemyBaseState>& aStateType);
	
	class UEnemyBehaviour& GetBehaviour() const;
	class UStaticMeshComponent& GetMesh() const;
	class AEnemy& GetSelf() const;
};