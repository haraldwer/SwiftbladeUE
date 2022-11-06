#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "States/EnemyBaseState.h"
#include "EnemyBehaviour.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyBehaviourStateChanged, UEnemyBaseState*, aState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyAnimationEvent, FString, aName);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyBehaviour : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyBehaviour();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetState(const TSubclassOf<UEnemyBaseState>& aClass);
	UEnemyBaseState* GetState() const { return myCurrentState.Get(); }
	bool CanDamageTarget() const;

	// - Targeting - //
	
	AActor* FindTarget() const;
	bool CheckTargetVisibility() const;
	bool CanAttackTarget() const;
	
	void SetTarget(AActor* aTarget) { myCurrentTarget = aTarget; }
	AActor* GetTarget() const { return myCurrentTarget.Get(); }

	void MoveTowards(const AActor* aTarget, float aMovementSpeed, float aForwardWeight, float aDT) const;
	void RotateTowards(const AActor* aTarget, float aRotationSpeed, float aDT) const;

	void OnTookDamage(float aDamageAmount, AActor* aDamageCauser);
	void OnDied(); 
	
protected:

	// - Targeting - //
	
	UPROPERTY(EditDefaultsOnly, Category="Behaviour")
	float myLookDot = 0.2;

	UPROPERTY(EditDefaultsOnly, Category="Behaviour")
	float myHearDistance = 750.0f;

	// Distance where targeting is possible
	UPROPERTY(EditDefaultsOnly, Category="Behaviour")
	float myTargetDistance = 1000.0f;

	// Distance before loosing sight of target 
	UPROPERTY(EditDefaultsOnly, Category="Behaviour")
	float myLookDistance = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category="Behaviour")
	float myAttackDistance = 500.0f;
		
	UPROPERTY(EditDefaultsOnly, Category="Behaviour")
	bool myCanTargetEnemies = false;

	UPROPERTY(EditDefaultsOnly, Category="Behaviour")
	bool myDrawDebugForces = false;

	UPROPERTY()
	TWeakObjectPtr<AActor> myCurrentTarget;

	// - Movement - //
	
	UPROPERTY(BlueprintAssignable)
	FOnEnemyBehaviourStateChanged myOnBehaviourStateChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnEnemyAnimationEvent myOnAnimationEvent;
	
private:
	bool CanTarget(const TWeakObjectPtr<AActor>& anActor, const FVector& aLocation, const FVector& aForward) const;

	void UpdateAnimations(UEnemyBaseState* aState, float aDT) const;
	
	TWeakObjectPtr<UEnemyBaseState> myCurrentState;
	TArray<TWeakObjectPtr<UEnemyBaseState>> myStates;
};
