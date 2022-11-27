#pragma once

#include "../SectionCompBase.h"
#include "SectionCompSpinBlade.generated.h"

UENUM()
enum class ESpinBladeLocation : uint8
{
	FLOOR,
	WALLS
};

UCLASS(Abstract)
class PROJECT_API USectionCompSpinBlade : public USectionCompBase
{
	GENERATED_BODY()

public:
	
	virtual void PopulateRoom(ASectionGenerator* aGenerator, const FProcSection& aSection, const FProcRoom& aRoom) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetRoll(AActor* aActor, float aRoll);

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> myBladeClass;

	UPROPERTY(EditAnywhere)
	ESpinBladeLocation myLocation = ESpinBladeLocation::FLOOR;

	UPROPERTY(EditAnywhere, Category="Walls")
	float myWallGroundPadding = 80.0f;

	UPROPERTY(EditAnywhere, Category="Walls")
	float myWallCeilPadding = 20.0f;

	UPROPERTY(EditAnywhere, Category="Walls")
	float myWallMaxHeight = 300.0f;

	UPROPERTY(EditAnywhere, Category="Walls")
	float myWallDist = 30.0f; 

	UPROPERTY(EditAnywhere, Category="Floor")
	float myFloorHeight = 20.0f; 
};
