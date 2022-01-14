#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project/Enemies/Behaviour/EnemyAnimation.h"
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

	FEnemyAnimation& GetAnimation() { return myAnimation; }  

protected:
	void SetState(const TSubclassOf<UEnemyBaseState>& aStateType);
	
	class UEnemyBehaviour& GetBehaviour() const;
	class UStaticMeshComponent& GetMesh() const;
	class AEnemy& GetSelf() const;

	UPROPERTY(EditDefaultsOnly)
	FEnemyAnimation myAnimation;
};