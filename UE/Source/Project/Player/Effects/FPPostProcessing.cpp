
#include "FPPostProcessing.h"

#include "Engine/PostProcessVolume.h"
#include "Materials/MaterialParameterCollectionInstance.h"

UFPPostProcessing::UFPPostProcessing()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFPPostProcessing::BeginPlay()
{
	Super::BeginPlay();

	for (auto& it : myPPs)
	{
		CHECK_CONTINUE_LOG(it.Key == PP_NONE, "Key none");
		CHECK_CONTINUE_LOG(!it.Value.myMaterial.Get(), "No material")
		
		const auto volume = Cast<APostProcessVolume>(GetWorld()->SpawnActor(APostProcessVolume::StaticClass()));
		CHECK_CONTINUE_LOG(!volume, "Failed to spawn volume");

		it.Value.myVolume = volume;
		volume->bUnbound = true;
		volume->bEnabled = it.Value.myDefaultEnable;
		FWeightedBlendable& blendable = volume->Settings.WeightedBlendables.Array.Emplace_GetRef();
		blendable.Weight = it.Value.myDefaultWeight;
		blendable.Object = it.Value.myMaterial;
	}
}

void UFPPostProcessing::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int32 i = myValues.Num() - 1; i >= 0; i--)
	{
		auto& it = myValues[i];
		
		// Set val
		UpdatePPValue(it.myPP, it.myName, it.myCurrentValue);
		
		// Interp current
		it.myCurrentValue = FMath::FInterpTo(it.myCurrentValue, it.myTargetValue, DeltaTime, myInterpSpeed);
		
		// Decrease target
		if (it.myDecrease)
			it.myTargetValue = FMath::FInterpTo(it.myTargetValue, 0.0f, DeltaTime, myInterpSpeed);
		
		// Remove PP? 
		if (FMath::Abs(it.myCurrentValue - it.myTargetValue) < KINDA_SMALL_NUMBER &&
			FMath::Abs(it.myTargetValue) < KINDA_SMALL_NUMBER &&
			it.myName == TEXT("Strength") || it.myName == TEXT("Weight"))
		{
			DisablePP(it.myPP);
			myValues.RemoveAt(i);
		}
	}
}

void UFPPostProcessing::SetPPScalar(const EFPPostProcess aPP, const FName aName, const float aValue, const bool aDecrease)
{
	const auto find = myValues.FindByPredicate([&](const PPValue& entry) {
		return (entry.myPP == aPP && entry.myName == aName);
	});

	if (!find)
	{
		myValues.Add({aPP, aName, aValue, 0.0f, aDecrease});
		EnablePP(aPP);
		return;
	}

	find->myTargetValue = aValue;
	find->myDecrease = aDecrease;
}


void UFPPostProcessing::EnablePP(const EFPPostProcess aPP)
{
	const auto volume = myPPs[aPP].myVolume;
	CHECK_RETURN_LOG(!volume, "Could not find volume");
	volume->bEnabled = true;
}

void UFPPostProcessing::DisablePP(const EFPPostProcess aPP)
{
	const auto volume = myPPs[aPP].myVolume;
	CHECK_RETURN_LOG(!volume, "Could not find volume");
	volume->bEnabled = false;
}

void UFPPostProcessing::UpdatePPValue(const EFPPostProcess aPP, const FName aName, const float aValue)
{
	const auto find = myPPs.Find(aPP);
	CHECK_RETURN_LOG(!find, "Could not find PP");

	// Special case for weight, optimized
	if (aName == "Weight")
	{
		find->myVolume->BlendWeight = aValue;
		return;
	}
	
	CHECK_RETURN_LOG(!find->myParamCollection, "No param collection assigned");
	const auto inst = GetWorld()->GetParameterCollectionInstance(find->myParamCollection);
	CHECK_RETURN_LOG(!inst, "No param collection instance found");
	if (!inst->SetScalarParameterValue(aName, aValue))
		LOG("Failed to set param " + aName.ToString());
}