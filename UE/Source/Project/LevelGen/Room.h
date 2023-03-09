#pragma once

#include "CoreMinimal.h"
#include "GeneratorBase.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Room.generated.h"

USTRUCT()
struct FRoomPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TWeakObjectPtr<AActor>> myActors;

	// TODO: 
	//UPROPERTY(EditAnywhere)
	//int32 myDifficulty;

	// TODO: 
	//UPROPERTY(EditAnywhere)
	//int32 myRequiredAbilities;
};

UCLASS(Abstract)
class PROJECT_API ARoom : public AGeneratorBase
{
	GENERATED_BODY()

public:

	ARoom();

	virtual void BeginPlay() override;
	virtual void Generate(ALevelManager* aLevelManager) override;
	
	FTransform GetEntry() const;
	FTransform GetExit() const;
	
	USplineComponent* GetPath() const { return myPath.Get(); }

protected:

	UFUNCTION(CallInEditor)
	void UpdatePath() const;
	
	UPROPERTY(EditAnywhere, Category="Connections")
	float myConnectionWidth = 300.0f;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> myRoot;
	UPROPERTY()
	TObjectPtr<USceneComponent> myEntry;
	UPROPERTY()
	TObjectPtr<UBoxComponent> myEntryMesh;
	UPROPERTY()
	TObjectPtr<USceneComponent> myExit;
	UPROPERTY()
	TObjectPtr<UBoxComponent> myExitMesh;
	UPROPERTY()
	TObjectPtr<USplineComponent> myPath;

	// Predefined obstacles and abilities

	UPROPERTY(EditAnywhere, Category="Presets")
	TArray<FRoomPreset> myPresets;

	UPROPERTY(EditAnywhere, Category="Presets")
	int32 myPresetIndex = -1; 
	
};
