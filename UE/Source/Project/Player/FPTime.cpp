#include "FPTime.h"
#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"

UFPTime::UFPTime()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFPTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float dilation = 1.0f;
	const float time = GetWorld()->GetRealTimeSeconds();
	TSet<EDilationType> toBeRemoved; 
	for (auto& entry : myEntries)
	{
		const float diff = time - entry.Value.myTimestamp;
		const auto curve = entry.Value.myCurve.Get();
		const float result = entry.Value.myValue * (curve ? curve->GetFloatValue(diff) : 1.0f);
		entry.Value.myLatestDilation = result; 
		dilation *= result;
		if (diff > entry.Value.myDuration && entry.Value.myDuration > 0.0f)
			toBeRemoved.Add(entry.Key);
	}

	for (const auto& remove : toBeRemoved)
		myEntries.Remove(remove); 
	
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), dilation);
}

void UFPTime::AddDilation(const EDilationType aType, const float aValue, const float aDuration, UCurveFloat* aCurve)
{
	const float time = GetWorld()->GetRealTimeSeconds();
	auto& entry = myEntries.FindOrAdd(aType);
	entry = { aValue, aDuration, time, aCurve, aValue };
}

void UFPTime::RemoveDilation(const EDilationType aType)
{
	if (myEntries.Contains(aType))
		myEntries.Remove(aType);
}

float UFPTime::GetDilationValue(const EDilationType aType) const
{
	if (const auto find = myEntries.Find(aType))
		return find->myLatestDilation;
	return -1.0f; 
}
