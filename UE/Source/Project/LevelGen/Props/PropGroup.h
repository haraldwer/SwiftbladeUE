#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PropGroup.generated.h"

class AGeneratorBase;

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
	void Generate(AGeneratorBase* aGenerator);

	UShapeComponent* GetVolume() const { return myVolume; }
	float GetWallOffset() const { return myWallOffset; }
	float GetYawRot() const { return myYawRot; }
	float GetMinRadius() const { return myMinPlacementRadius; }
	float GetMaxRadius() const { return myMaxPlacementRadius; }
	float GetHeightPart() const { return myHeightPart; }
	bool GetAgainstWall() const { return myAgainstWall; };

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
	bool myAgainstWall = true; 
	
	UPROPERTY(EditDefaultsOnly)
	float myMinPlacementRadius = -1.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myMaxPlacementRadius = -1.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myWallOffset = 0.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float myHeightPart = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float myYawRot = 360.0f;
	
};
