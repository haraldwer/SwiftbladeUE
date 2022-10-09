// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeneratorBase.generated.h"

UCLASS()
class PROJECT_API AGeneratorBase : public AActor
{
	GENERATED_BODY()

public:
	
	AGeneratorBase();
	virtual void BeginPlay() override;

	UFUNCTION(CallInEditor, Category="Generation")
	virtual void Generate();

	UFUNCTION(CallInEditor, Category="Generation")
	virtual void Clear();

	UFUNCTION(BlueprintCallable)
	void AddGeneratedObject(UObject* anObject);

	UFUNCTION(BlueprintCallable)
	AActor* SpawnGeneratedActor(const TSubclassOf<AActor>& anActorType, const FTransform& aTrans);

protected:

	#pragma optimize("", off)
	template <class T>
	static TArray<T*> GetComponents(const TArray<T*>& someComps)
	{
		CHECK_RETURN_LOG(!someComps.Num(), "No components", {});

		// Sort components based on requirements
		// Any requirement means that the required component has to come before
	
		TArray<T*> sortedComps;
		for (const auto comp : someComps)
			AddSortedComponent(someComps, sortedComps, comp, 0);
	
		// Filter out blocked components
		const auto isBlocked = [&](const T& aComp)
		{
			const auto& blockingComps = aComp.GetBlockingComps();
			for (auto& blockingComp : blockingComps)
			{
				const auto find = sortedComps.FindByPredicate(
					[&blockingComp, &aComp] (const T* aCompare)
					{
						if (aCompare != &aComp)
							return aCompare->IsA(blockingComp);
					   return false;
				   });
			
				if (find && *find)
					return true;
			}
			return false; 
		};
		TArray<T*> filteredComps;
		for (auto& comp : sortedComps)
		{
			CHECK_CONTINUE_LOG(!comp, "Component null");
			if (!isBlocked(*comp))
				filteredComps.Add(comp);
		}

		return filteredComps;
	}

	template <class T>
	static void AddSortedComponent(const TArray<T*>& aBase, TArray<T*>& aResult, T* aComp, const int32 aDepth)
	{
		CHECK_RETURN_LOG(!aComp, "Component null");
		CHECK_RETURN_LOG(aDepth > 10, "Recursive component dependency found, exiting loop");
		CHECK_RETURN(aResult.Contains(aComp));
		
		// This component requires these components to be prioritized above it
		const auto& requiredComps = aComp->GetReqiredComps();
		for (auto& requiredComp : requiredComps)
		{	
			// Add required comp before current comp
			// We've only got the type, find the actual object
			const auto objFind = aBase.FindByPredicate(
				[&requiredComp](const T* aCompare)
				{ return aCompare->IsA(requiredComp); });
			CHECK_RETURN_LOG(!objFind || !(*objFind), "Could not find required component in config components");
			AddSortedComponent(aBase, aResult, *objFind, aDepth + 1);
		}

		// Add this comp
		aResult.Add(aComp);
	}
	#pragma optimize("", on)
	
	UPROPERTY(EditAnywhere, Category="Generation")
	int32 mySeed = 0;

private:

	TMap<UClass*, TArray<TWeakObjectPtr<UObject>>> myGeneratedObjects;
	
};
