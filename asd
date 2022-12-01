
// Convert singeltons to subsystems? 

UINTERFACE()
class PooledActorInterface
{
	virtual void OnPoolActivate();
	virtual void OnPoolDeactivate();
}

UCLASS()
class UPoolManager : UActor
{
	GENERATED_BODY()

public:

	Create<T>(int32 aNumAllocated);
	Get<T>();

private:

	TMap<TSubclassOf<UActorPool>, TObjectPtr<UActorPool>> myPool;

}

UCLASS()
class UActorPool : public UObject
{
	GENERATED_BODY()

	friend class UPoolManager;

public:

	T* Borrow<T>();
	void Return<T>(T* anActor);

private:

	void Init(int32)
	{
		// Spawn and deactivate actors
		
	}

	TArray<int32> someUnusedIndices;
	TArray<AActor*> someActors; 
}

