#pragma once

#include "CoreMinimal.h"
#include "Project/LevelGen/GeneratorCompBase.h"
#include "Project/LevelGen/Section/SectionDataStructs.h"
#include "SectionCompBase.generated.h"

class ASectionGenerator;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class PROJECT_API USectionCompBase : public UGeneratorCompBase
{
	GENERATED_BODY()
	
public:

	// Return array of rooms to populate
	virtual TArray<int32> PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection);

	// Populate a specific room
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom);

protected:

	// For defining components in BP
	UFUNCTION(BlueprintImplementableEvent)
	void BPPopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom);

	// Helpers
	
	template <class T>
	static T* GetRoomComp(const FProcRoom& aRoom)
	{
		for (auto& comp : aRoom.components)
		{
			if (comp->IsA(T::StaticClass()))
				return Cast<T>(comp);
		}
		return nullptr;
	}

	static bool GetRandomCrossSection(const FProcRoom& aRoom, FVector2D& aFirst, FVector2D& aSecond);
	
};
