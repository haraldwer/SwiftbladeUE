#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PropGroup.generated.h"

UENUM()
enum class EPropPlacementStrategy : uint8
{
	PREDEFINED,
	USE_LOCAITONS,
	SCATTER
};

UCLASS()
class PROJECT_API APropGroup : public AActor
{
	GENERATED_BODY()

public:
	
	APropGroup();

	void Generate();
	
protected:

	UPROPERTY(EditDefaultsOnly)
	EPropPlacementStrategy myPlacementStrategy;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> myPropsToPlace;
	
};
