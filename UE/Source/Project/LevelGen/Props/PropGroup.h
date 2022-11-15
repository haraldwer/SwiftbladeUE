#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PropGroup.generated.h"

UENUM()
enum class EPropPlacementStrategy : uint8
{
	PREDEFINED,
	USE_LOCATIONS,
	SCATTER
};

UCLASS()
class PROJECT_API APropGroup : public AActor
{
	GENERATED_BODY()

public:
	
	APropGroup();
	void Generate(class AGeneratorBase* aGenerator);

	UShapeComponent* GetVolume() const { return myVolume; }
	float GetWallOffset() const { return myWallOffset; }
	float GetYawRot() const { return myYawRot; }

protected:
	
	FTransform GetScatterTrans() const;
	FTransform GetRandomLocationTrans(TArray<FTransform>& someUnusedLocations) const;
	TArray<FTransform> GetLocations() const;

	UPROPERTY(EditDefaultsOnly)
	EPropPlacementStrategy myPlacementStrategy;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> myPropsToPlace;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* myLocationParent = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UShapeComponent* myVolume = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* myRoot = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float myWallOffset = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float myYawRot = 360.0f;
	
};
