#pragma once

#include "CoreMinimal.h"

class UGeneratorCompBase;

template <class T>
struct FGeneratorCompEntry
{
	T* myPtr = nullptr;
	TArray<TSubclassOf<UGeneratorCompBase>>	myBlockingComps;
	TArray<TSubclassOf<UGeneratorCompBase>>	myRequiredComps;
};
