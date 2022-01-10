#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAnimator.generated.h"

UENUM(BlueprintType)
enum class EEnemyAnimationState : uint8
{
	
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UEnemyAnimator : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyAnimator();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	EEnemyAnimationState GetState() const { return myState; }
	
	void SetState(EEnemyAnimationState aN);
	
private:
	EEnemyAnimationState myState;
};
