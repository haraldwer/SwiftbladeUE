#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "States/EnemyBaseState.h"
#include "EnemyBehaviour.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyBehaviour : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyBehaviour();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetState(const TSubclassOf<UEnemyBaseState>& aClass);
	UEnemyBaseState* GetState() const { return myCurrentState; }
	bool CanDamageTarget() const;

	// - Targeting - //
	
	AActor* FindTarget();
	bool CheckTargetVisibility();
	bool CanAttackTarget() const;
	
	void SetTarget(AActor* aTarget) { myCurrentTarget = aTarget; }
	AActor* GetTarget() const { return myCurrentTarget; }

	void MoveTowards(AActor* aTarget, float aMovementSpeed, float aForwardWeight, float aDT) const;
	void RotateTowards(AActor* aTarget, float aRotationSpeed, float aDT) const;
	
protected:

	// - Targeting - //
	
	UPROPERTY(EditDefaultsOnly)
	float myLookDot = 0.2;

	UPROPERTY(EditDefaultsOnly)
	float myHearDistance = 750.0f;

	// Distance where targeting is possible
	UPROPERTY(EditDefaultsOnly)
	float myTargetDistance = 1000.0f;

	// Distance before loosing sight of target 
	UPROPERTY(EditDefaultsOnly)
	float myLookDistance = 1500.0f;

	UPROPERTY(EditDefaultsOnly)
	float myAttackDistance = 500.0f;
		
	UPROPERTY(EditDefaultsOnly)
	bool myCanTargetEnemies = false;

	UPROPERTY()
	AActor* myCurrentTarget;

	// - Movement - //
	
	UPROPERTY(EditDefaultsOnly)
	bool myGrounded = false;

private:
	bool CanTarget(AActor* anActor, const FVector& aLocation, const FVector& aForward);
	
	UPROPERTY()
	UEnemyBaseState* myCurrentState;
};
