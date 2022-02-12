#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGeneratedObject.generated.h"

UENUM(BlueprintType)
enum class EGeneratedObstacleType : uint8
{
	OBSTACLE_BIG,
	OBSTACLE_MEDIUM,
	OBSTACLE_SMALL,
	HAZARD,
	SPAWNER,
	PROP
};

USTRUCT(BlueprintType)
struct FLevelGeneratedObjectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSet<EGeneratedObstacleType> myTypes;
};

UCLASS()
class PROJECT_API ALevelGeneratedObject : public AActor
{
	GENERATED_BODY()

public:
	ALevelGeneratedObject();
	const FLevelGeneratedObjectData& GetData() const { return myData; }
	
protected:

	UPROPERTY(EditAnywhere)
	FLevelGeneratedObjectData myData;
};
