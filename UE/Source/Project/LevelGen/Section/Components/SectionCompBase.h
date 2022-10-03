#pragma once

#include "CoreMinimal.h"
#include "Project/LevelGen/Section/SectionDataStructs.h"
#include "SectionCompBase.generated.h"

class ASectionGenerator;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class PROJECT_API USectionCompBase : public UObject
{
	GENERATED_BODY()
	
public:

	// Return array of rooms to populate
	virtual TArray<int32> PopulateSection(ASectionGenerator* aGenerator, const FProcSection& aSection);

	// Populate a specific room
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom);

	const TArray<TSubclassOf<USectionCompBase>>& GetReqiredComps() const { return myRequiredComponents; }
	const TArray<TSubclassOf<USectionCompBase>>& GetBlockingComps() const { return myBlockingComponents; }
	
protected:

	// For defining components in BP
	UFUNCTION(BlueprintImplementableEvent)
	void BPPopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom);

	UPROPERTY(EditAnywhere)
	float myRoomChance = 100.0f;

	// Requirements
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<USectionCompBase>> myRequiredComponents;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<USectionCompBase>> myBlockingComponents;

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
