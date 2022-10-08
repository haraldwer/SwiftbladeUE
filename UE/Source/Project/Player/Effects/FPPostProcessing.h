#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPPostProcessing.generated.h"

UENUM(BlueprintType)
enum EFPPostProcess
{
	PP_NONE,
	PP_EYE,
	PP_MAGIC
};

USTRUCT(BlueprintType)
struct FPPEntry
{
	GENERATED_BODY()
	
	UPROPERTY()
	class APostProcessVolume* myVolume;

	UPROPERTY(EditDefaultsOnly, Category=FWeightedBlendable, meta=( AllowedClasses="BlendableInterface", Keywords="PostProcess" ))
	TObjectPtr<UObject> myMaterial;
	
	UPROPERTY(EditDefaultsOnly)
	float myDefaultWeight = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	bool myDefaultEnable = false;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialParameterCollection* myParamCollection;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_API UFPPostProcessing : public UActorComponent
{
	GENERATED_BODY()

public:
	UFPPostProcessing();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Assumes correct ParameterCollection in FPPEntry 
	// aDecrease : makes the value automatically lerp to zero
	UFUNCTION(BlueprintCallable)
	void SetPPScalar(const EFPPostProcess aPP, const FName aName, const float aValue, const bool aDecrease);

	// TODO: Custom interp speed 
	
protected:
	
	UPROPERTY(EditDefaultsOnly)
	TMap<TEnumAsByte<EFPPostProcess>, FPPEntry> myPPs;

	UPROPERTY(EditDefaultsOnly)
	float myInterpSpeed = 10.0f;
	
private:
	
	void EnablePP(EFPPostProcess aPP);
	void DisablePP(EFPPostProcess aPP);
	void UpdatePPValue(const EFPPostProcess aPP, const FName aName, const float aValue);
	
	struct PPValue
	{
		EFPPostProcess myPP = PP_NONE;
		FName myName;
		float myTargetValue = 0.0f;
		float myCurrentValue = 0.0f;
		bool myDecrease = false;
	};
	TArray<PPValue> myValues;
};
