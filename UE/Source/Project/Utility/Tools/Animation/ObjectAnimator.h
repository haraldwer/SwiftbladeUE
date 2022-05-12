#pragma once

#include "CoreMinimal.h"
#include "ObjectAnimatorKey.h"
#include "Components/ActorComponent.h"
#include "ObjectAnimator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UObjectAnimator : public UActorComponent
{
	GENERATED_BODY()

public:
	UObjectAnimator();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Update(float aDT);
	void SetKeys(const TArray<FObjectAnimatorKey>& someKeys);

protected:

	UPROPERTY(EditDefaultsOnly)
	float myFramerate = 30.0f;
	UPROPERTY(EditDefaultsOnly)
	float mySpeedMultiplier = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float myMinDist = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float myMinRot = 0.5f;
	
private:
	bool MoveObject(USceneComponent* anObject, USceneComponent* aTarget, float aSpeed, float aDT) const;
	
	UPROPERTY()
	TArray<USceneComponent*> myObjects;
	UPROPERTY()
	TArray<USceneComponent*> myTransforms;

	TArray<FObjectAnimatorKey> myKeys;
	bool myDone = false;
	float myFrameTimer = 0.0f;
};
