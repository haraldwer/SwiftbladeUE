#pragma once

#include "CoreMinimal.h"
#include "EnemyConfig.generated.h"

USTRUCT()
struct FEnemyWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AEnemy>> myEnemies;

	UPROPERTY(EditAnywhere)
	float myRequiredPercentage = 100.0f;
	
};

UCLASS(Blueprintable, EditInlineNew)
class PROJECT_API UEnemyConfig : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TArray<FEnemyWave> myWaves; 
	
};
