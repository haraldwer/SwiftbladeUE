#pragma once

#include "CoreMinimal.h"
#include "GeneratorStructs.h"
#include "GameFramework/Actor.h"
#include "GeneratorBase.generated.h"

UCLASS()
class PROJECT_API AGeneratorBase : public AActor
{
	GENERATED_BODY()

public:
	
	AGeneratorBase();

	UFUNCTION(CallInEditor, Category="Generation")
	void Generate() { Generate(nullptr); }
	
	virtual void Generate(class ALevelManager* aLevelManager);

	UFUNCTION(CallInEditor, Category="Generation")
	virtual void Clear();

	UFUNCTION(BlueprintCallable)
	void AddGeneratedObject(UObject* anObject);

	UFUNCTION(BlueprintCallable)
	AActor* SpawnGeneratedActor(const TSubclassOf<AActor>& anActorType, const FTransform& aTrans);

protected:

	template <class T>
	static void AddSortedComponent(const TArray<FGeneratorCompEntry<T>>& aBase, TArray<FGeneratorCompEntry<T>>& aResult, const FGeneratorCompEntry<T>& aComp, const int32 aDepth)
	{
		CHECK_RETURN_LOG(!aComp.myPtr, "Component null");
		CHECK_RETURN_LOG(aDepth > 10, "Recursive component dependency found, exiting loop");

		// Contains? 
		auto find = aResult.FindByPredicate(
			[&aComp](const FGeneratorCompEntry<T>& anEntry) { return anEntry.myPtr == aComp.myPtr; });
		CHECK_RETURN(find);
		
		// This component requires these components to be prioritized above it
		const TArray<TSubclassOf<UGeneratorCompBase>>& requiredComps = aComp.myRequiredComps;
		for (const TSubclassOf<UGeneratorCompBase>& requiredComp : requiredComps)
		{	
			// Add required comp before current comp
			// We've only got the type, find the actual object
			const auto objFind = aBase.FindByPredicate(
				[&requiredComp](const FGeneratorCompEntry<T>& aCompare)
				{ return aCompare.myPtr->IsA(requiredComp); });
			CHECK_RETURN(!objFind);
			AddSortedComponent(aBase, aResult, *objFind, aDepth + 1);
		}

		// Add this comp
		aResult.Add(aComp);
	}
	
	template <class T>
	static TArray<FGeneratorCompEntry<T>> GetComponents(const TArray<FGeneratorCompEntry<T>>& someComps)
	{
		CHECK_RETURN_LOG(!someComps.Num(), "No components", {});

		// Sort components based on requirements
		// Any requirement means that the required component has to come before
	
		TArray<FGeneratorCompEntry<T>> sortedComps;
		for (const FGeneratorCompEntry<T>& comp : someComps)
			AddSortedComponent(someComps, sortedComps, comp, 0);
	
		// Filter out blocked components
		const auto isBlocked = [&](const FGeneratorCompEntry<T>& aComp)
		{
			const TArray<TSubclassOf<UGeneratorCompBase>>& blockingComps = aComp.myBlockingComps;
			for (const TSubclassOf<UGeneratorCompBase>& blockingComp : blockingComps)
			{
				const FGeneratorCompEntry<T>* find = sortedComps.FindByPredicate(
					[&blockingComp, &aComp] (const FGeneratorCompEntry<T>& aCompare)
					{
						if (aCompare.myPtr != aComp.myPtr)
							return aCompare.myPtr->IsA(blockingComp);
					   return false;
				   });
			
				if (find && find->myPtr)
					return true;
			}
			return false; 
		};
		
		TArray<FGeneratorCompEntry<T>> filteredComps;
		for (const FGeneratorCompEntry<T>& comp : sortedComps)
		{
			CHECK_CONTINUE_LOG(!comp.myPtr, "Component null");
			if (!isBlocked(comp))
				filteredComps.Add(comp);
		}

		return filteredComps;
	}
	
	UPROPERTY(EditAnywhere, Category="Generation")
	int32 mySeed = 0;

private:

	TMap<UClass*, TArray<TWeakObjectPtr<UObject>>> myGeneratedObjects;
	
};
