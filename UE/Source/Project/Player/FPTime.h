#pragma once

#include "CoreMinimal.h"
#include "FPComponentBase.h"
#include "Components/ActorComponent.h"
#include "FPTime.generated.h"

UENUM(BlueprintType)
enum class EDilationType : uint8
{
	OUTGOING_DAMAGE,
	INTRO,
	DEATH,
	CHECKPOINT,
	PAUSE
};

class UCurveFloat; 

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPTime : public UFPActorComponentBase
{
	GENERATED_BODY()

public:
	
	UFPTime();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetInitialTime(float aTime);
	float GetScoreTime() const;

	UFUNCTION(BlueprintCallable)
	void AddDilation(EDilationType aType, float aValue, float aDuration = -1.0f, UCurveFloat* aCurve = nullptr);

	UFUNCTION(BlueprintCallable)
	void RemoveDilation(EDilationType aType);

	UFUNCTION(BlueprintPure)
	float GetDilationValue(EDilationType aType) const;

private:

	struct Entry
	{
		float myValue = 1.0f;
		float myDuration = -1.0f; 
		float myTimestamp = -1.0f; 
		TWeakObjectPtr<UCurveFloat> myCurve = nullptr;
		float myLatestDilation = 1.0f; 
	};
	
	TMap<EDilationType, Entry> myEntries;

	float myInitialTime = 0.0f;
	
};
