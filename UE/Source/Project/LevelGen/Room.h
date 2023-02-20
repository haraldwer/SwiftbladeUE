#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Room.generated.h"

UCLASS(Abstract)
class PROJECT_API ARoom : public AActor
{
	GENERATED_BODY()

public:

	ARoom();
	
	UFUNCTION(CallInEditor)
	void UpdatePath() const;

	UFUNCTION(CallInEditor)
	void PreviewRadius() const;
	
	FTransform GetEntry() const;
	FTransform GetExit() const;
	
	FVector GetExitDirection() const;
	FVector GetCenter() const;
	float GetRadius() const;
	USplineComponent* GetPath() const { return myPath.Get(); }

protected:

	UPROPERTY(EditAnywhere, Category="Connections")
	float myConnectionWidth = 300.0f;
	UPROPERTY(EditAnywhere, Category="Connections")
	TObjectPtr<USceneComponent> myRoot;
	UPROPERTY(EditAnywhere, Category="Connections")
	TObjectPtr<USceneComponent> myEntry;
	UPROPERTY(EditAnywhere, Category="Connections")
	TObjectPtr<UBoxComponent> myEntryMesh;
	UPROPERTY(EditAnywhere, Category="Connections")
	TObjectPtr<USceneComponent> myExit;
	UPROPERTY(EditAnywhere, Category="Connections")
	TObjectPtr<UBoxComponent> myExitMesh;
	UPROPERTY(EditAnywhere, Category="Connections")
	TObjectPtr<USplineComponent> myPath;

	// Predefined obstacles and abilities
	
};
