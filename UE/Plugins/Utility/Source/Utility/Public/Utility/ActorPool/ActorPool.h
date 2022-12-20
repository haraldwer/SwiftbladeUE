#pragma once

#include "PooledActorInterface.h"
#include "ActorPool.generated.h"

UCLASS()
class UActorPool : public UObject
{
	GENERATED_BODY()

	friend class UPoolManager;

public:

	template <class T>
	T* Borrow()
	{
		CHECK_RETURN_LOG(!myUnusedIndices.Num(), "No unused actors in pool", nullptr);
		const int32 index = myUnusedIndices.Pop();
		AActor* actor = myActors[index];
		if (const auto interface = Cast<IPooledActorInterface>(actor))
			interface->OnPoolActivate();
		return (*actor);
	}
	
	template <class T>
	void Return(T* anActor)
	{
		CHECK_RETURN_LOG(!anActor, "Actor null on return");
		if (const auto interface = Cast<IPooledActorInterface>(anActor))
			interface->OnPoolDeactivate();
		int32 find = 0;
		if (myActors.Find(anActor, find))
			myUnusedIndices.Add(find);
	}

private:

	template <class T>
	void Init(const int32 aNum)
	{
		// Spawn and deactivate actors
		for (int32 i = 0; i < aNum; i++)
		{
			T* newActor = GetWorld()->SpawnActor<T>();
			myActors.Add(newActor);
			myUnusedIndices.Add(i);
			if (auto interface = Cast<IPooledActorInterface>(newActor))
				interface->OnPoolDeactivate();
		}		
	}

	UPROPERTY(Transient)
	TArray<int32> myUnusedIndices;
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> myActors; 
};


UCLASS()
class APoolManager : public AActor
{
	GENERATED_BODY()

public:

	template <class T>
	void Create(int32 aNumAllocated)
	{
		TObjectPtr<UActorPool>& poolPtr = myPools.FindOrAdd(T::StaticClass());
		if (poolPtr.IsNull())
			poolPtr = NewObject<UActorPool>();
		poolPtr->Init<T>(aNumAllocated);
	}

	template <class T>
	UActorPool& Get()
	{
		auto ptr = myPools.FindOrAdd(T::StaticClass()).Get();
		CHECK_ASSERT(!ptr, "No pool of type");
		return *ptr;
	}

private:
	
	TMap<TSubclassOf<UActorPool>, TObjectPtr<UActorPool>> myPools;

};

