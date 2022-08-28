#pragma once

#include "CoreMinimal.h"
#include "../LevelDataStructs.h"
#include "LevelDataConfig.generated.h"

class UProceduralMeshComponent;
class ALevelData;

UCLASS(Blueprintable)
class ULevelDataConfig : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category="Generation")
	int32 myCells = 10;
	UPROPERTY(EditAnywhere, Category="Generation")
	float myNoiseRadius = 2000.0f;
	UPROPERTY(EditAnywhere, Category="Generation")
	float myConnectionPadding = 200.0f;
	
	UPROPERTY(EditAnywhere, Category="Ground")
	bool myHasGround;
	UPROPERTY(EditAnywhere, Category="Ground", meta=(EditCondition=myHasGround))
	float myGroundThickness = 5000.0f;
	UPROPERTY(EditAnywhere, Category="Ground", meta=(EditCondition=myHasGround))
	float myGroundMinOffset = -500.0f;
	UPROPERTY(EditAnywhere, Category="Ground", meta=(EditCondition=myHasGround))
	float myGroundMaxOffset = 500.0f;
	UPROPERTY(EditAnywhere, Category="Ground", meta=(EditCondition=myHasGround))
	UMaterialInterface* myGroundMaterial;
	
	UPROPERTY(EditAnywhere, Category="Ceiling")
	bool myHasCeil;
	UPROPERTY(EditAnywhere, Category="Ceiling", meta=(EditCondition=myHasCeil))
	float myCeilingThickness = 5000.0f;
	UPROPERTY(EditAnywhere, Category="Ceiling", meta=(EditCondition=myHasCeil))
	float myCeilingMinHeight = 500.0f;
	UPROPERTY(EditAnywhere, Category="Ceiling", meta=(EditCondition=myHasCeil))
	float myCeilingMaxHeight = 500.0f;
	UPROPERTY(EditAnywhere, Category="Ceiling", meta=(EditCondition=myHasCeil))
	UMaterialInterface* myCeilingMaterial;
	
	virtual void Populate(ALevelData* aLevelData, const TMap<int32, FLevelDataFace>& someFaces, const TArray<int32> aPath, const FVector& anOffset);
	virtual void PopulateFace(ALevelData* aLevelData, const FLevelDataFace& aFace, const FVector& anOffset);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateWall(ALevelData* aLevelData, const TArray<FVector2D>& someVertices, const FVector& anOffset, float aWallOffset, float aWallHeight);
	
};
