#pragma once

#include "CoreMinimal.h"
#include "Project/LevelGen/Section/SectionDataStructs.h"
#include "SectionComponentBase.generated.h"

class ASectionGenerator;

UCLASS(Blueprintable)
class PROJECT_API USectionComponentBase : public UObject
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

	UPROPERTY(EditAnywhere)
	float myRoomChance = 100.0f;

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
	
};
