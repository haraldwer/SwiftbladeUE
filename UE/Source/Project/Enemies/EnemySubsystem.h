#pragma once

#include "CoreMinimal.h"
#include "EnemySubsystem.generated.h"

class UEnemyConfig;
class AEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemiesCleared);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveBegun, int32, aWave);

UCLASS()
class PROJECT_API UEnemySubsystem : public UWorldSubsystem 
{
	GENERATED_BODY()

public:
	
	static UEnemySubsystem& Get();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void Init(UEnemyConfig* aConfig);
	
	void CheckWaveConditions();
	void InitWave(int32 anIndex);

	void AddEnemy(AEnemy* anEnemy);
	void RemoveEnemy(AEnemy* anEnemy);
	
	const TSet<TWeakObjectPtr<AEnemy>>& GetEnemies() const { return myEnemies; }

	bool GetIsLastEnemy() const; 

	UPROPERTY(BlueprintAssignable)
	FOnEnemiesCleared myOnEnemiesCleared;

	UPROPERTY(BlueprintAssignable)
	FOnWaveBegun myOnWaveBegun; 

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEnemyConfig> myDefaultConfig; 

private:

	TSet<TWeakObjectPtr<AEnemy>> myEnemies;
	TObjectPtr<UEnemyConfig> myConfig;
	int32 myWave = -1;
	
};
