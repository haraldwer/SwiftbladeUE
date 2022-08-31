#pragma once

#include "CoreMinimal.h"
#include "../LevelDataStructs.h"
#include "LevelDataConfig.generated.h"

class UProceduralMeshComponent;
class ALevelData;

USTRUCT()
struct FLevelProp
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> myClass;
	UPROPERTY(EditDefaultsOnly)
	float mySpawnWeight = 1.0f; // 1 == as common as any other prop
	UPROPERTY(EditDefaultsOnly)
	float myHeight = 0.0f;
	UPROPERTY(EditDefaultsOnly)
	float myPadding = 0.0f;
};

UCLASS(Blueprintable)
class ULevelPropConfig : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly)
	float myMinFillRate = 0.0f;
	UPROPERTY(EditDefaultsOnly)
	float myMaxFillRate = 10.0f;
	UPROPERTY(EditDefaultsOnly)
	TArray<FLevelProp> myProps;
	
};

UCLASS(Blueprintable)
class ULevelDataConfig : public UObject
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, Category="Generation")
	int32 myNumRooms = 10;
	UPROPERTY(EditAnywhere, Category="Generation")
	int32 myMinNumVerts = 4;
	UPROPERTY(EditAnywhere, Category="Generation")
	int32 myMaxNumVerts = 7;
	UPROPERTY(EditAnywhere, Category="Generation")
	float myMinRoomRadius = 800.0f;
	UPROPERTY(EditAnywhere, Category="Generation")
	float myMaxRoomRadius = 1200.0f;
	UPROPERTY(EditAnywhere, Category="Generation")
	bool mySnakePath = true;
	UPROPERTY(EditAnywhere, Category="Generation")
	float mySnakePathY = 0.2f;
	
	UPROPERTY(EditAnywhere, Category="Ground")
	bool myHasGround = true;
	UPROPERTY(EditAnywhere, Category="Ground", meta=(EditCondition=myHasGround))
	float myGroundThickness = 5000.0f;
	UPROPERTY(EditAnywhere, Category="Ground", meta=(EditCondition=myHasGround))
	float myGroundMinOffset = -500.0f;
	UPROPERTY(EditAnywhere, Category="Ground", meta=(EditCondition=myHasGround))
	float myGroundMaxOffset = 500.0f;
	UPROPERTY(EditAnywhere, Category="Ground", meta=(EditCondition=myHasGround))
	UMaterialInterface* myGroundMaterial;
	
	UPROPERTY(EditAnywhere, Category="Ceiling")
	bool myHasCeil = true;
	UPROPERTY(EditAnywhere, Category="Ceiling", meta=(EditCondition=myHasCeil))
	float myCeilingThickness = 5000.0f;
	UPROPERTY(EditAnywhere, Category="Ceiling", meta=(EditCondition=myHasCeil))
	float myCeilingMinHeight = 500.0f;
	UPROPERTY(EditAnywhere, Category="Ceiling", meta=(EditCondition=myHasCeil))
	float myCeilingMaxHeight = 500.0f;
	UPROPERTY(EditAnywhere, Category="Ceiling", meta=(EditCondition=myHasCeil))
	UMaterialInterface* myCeilingMaterial;

	UPROPERTY(EditAnywhere, Category="Propping")
	TSubclassOf<ULevelPropConfig> myPropConfig;
	
	virtual void Populate(ALevelData* aLevelData, const TArray<FLevelDataRoom>& someRooms);
	virtual void PopulateRoom(ALevelData* aLevelData, const FLevelDataRoom& aRoom);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateWall(ALevelData* aLevelData, const TArray<FVector2D>& someVertices, float aWallOffset, float aWallHeight);
	
};
