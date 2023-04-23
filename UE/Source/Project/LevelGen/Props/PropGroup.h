#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PropGroup.generated.h"

class UShapeComponent;
class AGeneratorBase;

UENUM()
enum class EPropPlacementStrategy : uint8
{
	PREDEFINED,
	USE_LOCATIONS,
	SCATTER
};

UENUM()
enum class EPropPlacementMode : uint8
{
	WALL,
	CORNER
};

UCLASS()
class PROJECT_API APropGroup : public AActor
{
	GENERATED_BODY()

public:
	
	APropGroup();
	void Generate(AGeneratorBase* aGenerator);

	UShapeComponent* GetVolume() const { return myVolume; }
	float GetWallOffset() const { return myWallOffset; }
	float GetCornerDist() const { return myCornerDist; }
	float GetYawRot() const { return myYawRot; }
	float GetMinWallDist() const { return myMinWallDist; }
	float GetMaxWallDist() const { return myMaxWallDist; }

	float GetMinHeight() const { return myMinHeight; }
	float GetMaxHeight() const { return myMaxHeight; }
	float GetMinCeilDist() const { return myMinCeilDist; }
	
	EPropPlacementMode GetPlacementMode() const { return myPlacementMode; }

protected:

	void PlaceProp(AGeneratorBase* aGenerator, TArray<FTransform>& someUnusedLocations, UObject* aDefaultObject);
	
	FTransform GetScatterTrans() const;
	FTransform GetRandomLocationTrans(TArray<FTransform>& someUnusedLocations) const;
	TArray<FTransform> GetLocations() const;

	UPROPERTY(EditDefaultsOnly)
	EPropPlacementStrategy myPlacementStrategy;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> myActorsToPlace;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UStaticMesh>> myMeshesToPlace;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* myLocationParent = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UShapeComponent* myVolume = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* myRoot = nullptr;

	UPROPERTY(EditDefaultsOnly)
	EPropPlacementMode myPlacementMode = EPropPlacementMode::WALL;
	
	UPROPERTY(EditDefaultsOnly)
	float myMinWallDist = 0.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myMaxWallDist = 0.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myWallOffset = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float myCornerDist = 20.0f;

	UPROPERTY(EditDefaultsOnly)
	float myYawRot = 360.0f;

	UPROPERTY(EditDefaultsOnly)
	float myMinHeight = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float myMaxHeight = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float myMinCeilDist = 0.0f;
	
};
